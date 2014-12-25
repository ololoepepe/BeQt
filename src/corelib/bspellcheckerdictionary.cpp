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

#include "bbase_p.h"
#include "bdirtools.h"

#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QFileInfo>
#include <QLocale>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTextCodec>

#if defined(BUILDING_LIBHUNSPELL)
#include "../3rdparty/hunspell/hunspell.hxx"
#else
#include <hunspell/hunspell.hxx>
#endif

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

class BSpellCheckerDictionaryPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSpellCheckerDictionary)
public:
    static QMap<QChar, QChar> replacedLetters;
public:
    const QByteArray AffixData;
    const QByteArray DictionaryData;
    const QLocale Locale;
    const QString Path;
public:
    QTextCodec *codec;
    Hunspell *hunspell;
public:
    explicit BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path,
                                            const QByteArray &affixData = QByteArray(),
                                            const QByteArray &dictionaryData = QByteArray(),
                                            const QString &locale = QString());
    ~BSpellCheckerDictionaryPrivate();
public:
    static QString replaceSpecialLetters(QString w);
public:
    void init();
    bool testValidity();
private:
    Q_DISABLE_COPY(BSpellCheckerDictionaryPrivate)
};

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

/*============================== Static public variables ===================*/

QMap<QChar, QChar> BSpellCheckerDictionaryPrivate::replacedLetters;

/*============================== Public constructors =======================*/

BSpellCheckerDictionaryPrivate::BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path,
                                                               const QByteArray &affixData,
                                                               const QByteArray &dictionaryData,
                                                               const QString &locale) :
    BBasePrivate(q), AffixData(affixData), DictionaryData(dictionaryData),
    Locale(!locale.isEmpty() ? locale : QFileInfo(path).fileName()), Path(path)
{
    //
}

BSpellCheckerDictionaryPrivate::~BSpellCheckerDictionaryPrivate()
{
    delete hunspell;
}

/*============================== Static public methods =====================*/

QString BSpellCheckerDictionaryPrivate::replaceSpecialLetters(QString w)
{
    w.replace(L'\u0451', L'\u0435').replace(L'\u0401', L'\u0415'); //Russian 'ё' and 'Ё' are treated as 'е' and 'Е'
    foreach (const QChar &key, replacedLetters.keys())
        w.replace(key, replacedLetters.value(key));
    return w;
}

/*============================== Public methods ============================*/

void BSpellCheckerDictionaryPrivate::init()
{
    hunspell = 0;
    codec = 0;
    if (Path.isEmpty() && (AffixData.isEmpty() || DictionaryData.isEmpty()))
        return;
    if (Path.isEmpty()) {
        hunspell = new Hunspell(AffixData.constData(), DictionaryData.constData(), 0, true, true);
    } else {
        QString aff = Path + "/" + Locale.name() + ".aff";
        QString dic = Path + "/" + Locale.name() + ".dic";
        if (Path.startsWith(":/")) {
            QByteArray affdata = BDirTools::readFile(aff);
            QByteArray dicdata = BDirTools::readFile(dic);
            hunspell = new Hunspell(affdata.constData(), dicdata.constData(), 0, true, true);
            //Warning: hzipped files can not be loaded here. Unzip them first
        } else {
            hunspell = new Hunspell(aff.toLocal8Bit().constData(), dic.toLocal8Bit().constData());
        }
    }
    codec = QTextCodec::codecForName(hunspell->get_dic_encoding());
    if (!codec || !testValidity()) {
        delete hunspell;
        hunspell = 0;
        codec = 0;
    }
}

bool BSpellCheckerDictionaryPrivate::testValidity()
{
    QStringList sl = Locale.nativeLanguageName().split(' ');
    return !sl.isEmpty() && q_func()->spell(sl);
}

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

/*!
\class BSpellCheckerDictionary
\inmodule BeQtCore
\brief The BSpellCheckerDictionary class provides a dictionary for spell checking.

The BSpellCheckerDictionary is a wrapper over a Hunspell dictionary. It is used to check spelling of words.

\sa BSpellChecker
*/

/*============================== Public constructors =======================*/

/*!
Constructs a dictionary and sets the path to dictionary directory to \a path. The dictionary is loaded automatically
from that directory. The locale is set according to the directory name.
*/

BSpellCheckerDictionary::BSpellCheckerDictionary(const QString &path) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, path))
{
    d_func()->init();
}

/*!
Constructs a dictionary and sets the underlying Hunspell dictionary affix data to \a affixData and dictionary data to
\a dictionaryData. The locale is set to the locale with name \a locale.
*/

BSpellCheckerDictionary::BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                                 const QString &locale) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, "", affixData, dictionaryData, locale))
{
    d_func()->init();
}

/*!
Constructs a dictionary and sets the underlying Hunspell dictionary affix data to \a affixData and dictionary data to
\a dictionaryData. The locale is set to \a locale.
*/

BSpellCheckerDictionary::BSpellCheckerDictionary(const QByteArray &affixData, const QByteArray &dictionaryData,
                                                 const QLocale &locale) :
    BBase(*new BSpellCheckerDictionaryPrivate(this, "", affixData, dictionaryData, locale.name()))
{
    d_func()->init();
}

/*!
Destroys the object, deleting the associated data object.

The underlying Hunspell dictionary is also deleted.
*/

BSpellCheckerDictionary::~BSpellCheckerDictionary()
{
    //
}

/*============================== Static public methods =====================*/

/*!
Returns a map of the characters (typically letters) which should be replaced in a string before checking spelling. This
is useful, for example, for such cases as Russian 'ё' letter, which must be treated as 'е' for Hunspell to work
correctly.

Some of such letter replacements are hardcoded into BeQt library (e.g. the above mentioned 'ё').

\sa setReplacedLetters()
*/

QMap<QChar, QChar> BSpellCheckerDictionary::replacedLetters()
{
    return BSpellCheckerDictionaryPrivate::replacedLetters;
}

/*!
Sets the map of the characters (typically letters) which should be replaced in a string before checking spelling to
\a m.

\sa replacedLetters()
*/

void BSpellCheckerDictionary::setReplacedLetters(const QMap<QChar, QChar> &m)
{
    BSpellCheckerDictionaryPrivate::replacedLetters = m;
}

/*============================== Public methods ============================*/

/*!
Adds the \a word to this dictionary. That word will be treated as correct when calling spell().
*/

void BSpellCheckerDictionary::addWord(const QString &word)
{
    if (!isValid())
        return;
    const char *w = codec()->fromUnicode(BSpellCheckerDictionaryPrivate::replaceSpecialLetters(word)).constData();
     d_func()->hunspell->add(w);
}

/*!
Returns the raw affix data of the dictionary.
*/

QByteArray BSpellCheckerDictionary::affixData() const
{
    return d_func()->AffixData;
}

/*!
Returns a pointer to the QTextCodec used to decode the dictionary.

For an invalid BSpellCheckerDictionary instance this function returns 0.
*/

QTextCodec *BSpellCheckerDictionary::codec() const
{
    return d_func()->codec;
}

/*!
Returns the name of the QTextCodec used to decode the dictionary.

For an invalid BSpellCheckerDictionary instance this function returns 0.
*/

QString BSpellCheckerDictionary::codecName() const
{
    return d_func()->codec ? QString::fromLatin1(d_func()->codec->name()) : QString();
}

/*!
Returns the raw dictionary data of the dictionary.
*/

QByteArray BSpellCheckerDictionary::dictionaryData() const
{
    return d_func()->DictionaryData;
}

/*!
Returns true if this dictionary instance is valid (i.e. the underlying Hunspell dictionary is valid). Otherwise returns
false.
*/

bool BSpellCheckerDictionary::isValid() const
{
    return d_func()->hunspell;
}

/*!
Returns the locale of the dictionary.
*/

QLocale BSpellCheckerDictionary::locale() const
{
    return d_func()->Locale;
}

/*!
Returns the name of the locale of the dictionary.
*/

QString BSpellCheckerDictionary::localeName() const
{
    return isValid() ? d_func()->Locale.name() : QString();
}

/*!
Returns the path to the directory from which this dictionary was loaded.

If the dictionary was loaded from raw data, this function returns an empty QString.
*/

QString BSpellCheckerDictionary::path() const
{
    return d_func()->Path;
}

/*!
Removes the \a word from this dictionary. That word will be treated as incorrect when calling spell().
*/

void BSpellCheckerDictionary::removeWord(const QString &word)
{
    if (!isValid())
        return;
    const char *w = codec()->fromUnicode(BSpellCheckerDictionaryPrivate::replaceSpecialLetters(word)).constData();
     d_func()->hunspell->remove(w);
}

/*!
Checks if the \a word is correct (i.e. the dictionary contains it). In case the word is correct, returns true;
otherwise returns false.
*/

bool BSpellCheckerDictionary::spell(const QString &word) const
{
    if (!isValid())
        return false;
    const char *w = codec()->fromUnicode(BSpellCheckerDictionaryPrivate::replaceSpecialLetters(word)).constData();
    return d_func()->hunspell->spell(w);
}

/*!
Checks if all the \a words are correct (i.e. the dictionary contains them all). In case all the words are correct,
returns true. If any word is incorrect, returns false.
*/

bool BSpellCheckerDictionary::spell(const QStringList &words) const
{
    foreach (const QString &word, words) {
        if (!spell(word))
            return false;
    }
    return true;
}

/*!
Returns a list of the words resembling the given \a word.

This function may be used to suggest a list of possible variants of corrections for a word written incorrectly.

For example, for the incorrectly written word "spon" the following words will be suggested: "soon", "spin", "son",
"spoon", "pons", "span", "spot", "spun", "upon", "sponge".
*/

QStringList BSpellCheckerDictionary::suggest(const QString &word) const
{
    if (!isValid())
        return QStringList();
    char ***list = new char **;
    QString w = BSpellCheckerDictionaryPrivate::replaceSpecialLetters(word);
    int count = d_func()->hunspell->suggest(list, d_func()->codec->fromUnicode(w).constData());
    QStringList sl;
    foreach (int i, bRangeD(0, count - 1))
        sl << d_func()->codec->toUnicode((*list)[i]);
    d_func()->hunspell->free_list(list, count);
    return sl;
}
