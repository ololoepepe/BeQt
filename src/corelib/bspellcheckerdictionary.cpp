/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bspellcheckerdictionary.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"
#include "bdirtools.h"

#if defined(BUILDING_LIBHUNSPELL)
#include "../3rdparty/hunspell/hunspell.hxx"
#else
#include <hunspell/hunspell.hxx>
#endif

#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QByteArray>
#include <QFileInfo>
#include <QLocale>

#include <QDebug>

static QString replaceSpecialLetters(QString w)
{
    return w.replace(L'\u0451', L'\u0435').replace(L'\u0401', L'\u0415');
}

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

class BSpellCheckerDictionaryPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSpellCheckerDictionary)
public:
        explicit BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path,
                                                const QByteArray &affixData = QByteArray(),
                                                const QByteArray &dictionaryData = QByteArray(),
                                                const QString &locale = QString());
    ~BSpellCheckerDictionaryPrivate();
public:
    void init();
    bool testValidity();
public:
    const QString Path;
    const QByteArray AffixData;
    const QByteArray DictionaryData;
    const QLocale Locale;
public:
    Hunspell *hunspell;
    QTextCodec *codec;
private:
    Q_DISABLE_COPY(BSpellCheckerDictionaryPrivate)
};

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

/*============================== Public constructors =======================*/

BSpellCheckerDictionaryPrivate::BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path,
                                                               const QByteArray &affixData,
                                                               const QByteArray &dictionaryData,
                                                               const QString &locale) :
    BBasePrivate(q), Path(path), AffixData(affixData), DictionaryData(dictionaryData),
    Locale(!locale.isEmpty() ? locale : QFileInfo(path).fileName())
{
    //
}

BSpellCheckerDictionaryPrivate::~BSpellCheckerDictionaryPrivate()
{
    delete hunspell;
}

/*============================== Public methods ============================*/

void BSpellCheckerDictionaryPrivate::init()
{
    hunspell = 0;
    codec = 0;
    if (Path.isEmpty() && (AffixData.isEmpty() || DictionaryData.isEmpty()))
        return;
    if (Path.isEmpty())
    {
        hunspell = new Hunspell(AffixData.constData(), DictionaryData.constData(), 0, true, true);
    }
    else
    {
        QString aff = Path + "/" + Locale.name() + ".aff";
        QString dic = Path + "/" + Locale.name() + ".dic";
        if (Path.startsWith(":/"))
        {
            QByteArray affdata = BDirTools::readFile(aff);
            QByteArray dicdata = BDirTools::readFile(dic);
            hunspell = new Hunspell(affdata.constData(), dicdata.constData(), 0, true, true);
            //Warning: hzipped files can not be loaded here. Unzip them first
        }
        else
        {
            hunspell = new Hunspell(aff.toLocal8Bit().constData(), dic.toLocal8Bit().constData());
        }
    }
    codec = QTextCodec::codecForName(hunspell->get_dic_encoding());
    if (!codec || !testValidity())
    {
        delete hunspell;
        hunspell = 0;
        codec = 0;
    }
}

bool BSpellCheckerDictionaryPrivate::testValidity()
{
    //Testing Hunspell instance using the word "test" in corresponding language
    if (Locale.name().startsWith("en"))
        return q_func()->spell("test");
    else if (Locale.name().startsWith("ru"))
        return q_func()->spell(QString::fromWCharArray(L"\u0442\u0435\u0441\u0442"));
    else
        return true; //Actually, returning true hers is not cool
}

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

/*============================== Public constructors =======================*/

BSpellCheckerDictionary::BSpellCheckerDictionary(const QString &path) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, path))
{
    d_func()->init();
}

BSpellCheckerDictionary::BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                                 const QString &locale) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, "", affixData, dictionaryData, locale))
{
    d_func()->init();
}

BSpellCheckerDictionary::BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                                 const QLocale &locale) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, "", affixData, dictionaryData, locale.name()))
{
    d_func()->init();
}

BSpellCheckerDictionary::~BSpellCheckerDictionary()
{
    //
}

/*============================== Public methods ============================*/

bool BSpellCheckerDictionary::spell(const QString &word) const
{
    if (!isValid())
        return false;
    return d_func()->hunspell->spell(codec()->fromUnicode(replaceSpecialLetters(word)).constData());
}

QStringList BSpellCheckerDictionary::suggest(const QString &word) const
{
    if (!isValid())
        return QStringList();
    char ***list = new char **;
    int count = d_func()->hunspell->suggest(list,
                                            d_func()->codec->fromUnicode(replaceSpecialLetters(word)).constData());
    QStringList sl;
    foreach (int i, bRangeD(0, count - 1))
        sl << d_func()->codec->toUnicode((*list)[i]);
    d_func()->hunspell->free_list(list, count);
    return sl;
}

void BSpellCheckerDictionary::addWord(const QString &word)
{
    if (!isValid())
        return;
     d_func()->hunspell->add(codec()->fromUnicode(replaceSpecialLetters(word)).constData());
}

void BSpellCheckerDictionary::removeWord(const QString &word)
{
    if (!isValid())
        return;
     d_func()->hunspell->remove(codec()->fromUnicode(replaceSpecialLetters(word)).constData());
}

QString BSpellCheckerDictionary::path() const
{
    return d_func()->Path;
}

QByteArray BSpellCheckerDictionary::affixData() const
{
    return d_func()->AffixData;
}

QByteArray BSpellCheckerDictionary::dictionaryData() const
{
    return d_func()->DictionaryData;
}

QLocale BSpellCheckerDictionary::locale() const
{
    return d_func()->Locale;
}

QString BSpellCheckerDictionary::localeName() const
{
    return isValid() ? d_func()->Locale.name() : QString();
}

QTextCodec *BSpellCheckerDictionary::codec() const
{
    return d_func()->codec;
}

QString BSpellCheckerDictionary::codecName() const
{
    return d_func()->codec ? QString::fromLatin1(d_func()->codec->name()) : QString();
}

bool BSpellCheckerDictionary::isValid() const
{
    return d_func()->hunspell;
}
