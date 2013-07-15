#include "bspellcheckerdictionary.h"
#include "bglobal.h"
#include "bbase_p.h"

#if defined(BUILDING_LIBHUNSPELLS)
#include "hunspell/hunspell.hxx"
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

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

class BSpellCheckerDictionaryPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path);
    ~BSpellCheckerDictionaryPrivate();
public:
    void init();
    bool testValidity();
public:
    const QString Path;
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

BSpellCheckerDictionaryPrivate::BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q, const QString &path) :
    BBasePrivate(q), Path(path), Locale(QFileInfo(path).fileName())
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
    if (Path.isEmpty() || QFileInfo(Path).fileName() != Locale.name())
        return;
    QString aff = Path + "/" + Locale.name() + ".aff";
    QString dic = Path + "/" + Locale.name() + ".dic";
    if (!QFileInfo(aff).isFile() || !QFileInfo(dic).isFile())
        return;
    hunspell = new Hunspell(aff.toLocal8Bit().constData(), dic.toLocal8Bit().constData());
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

BSpellCheckerDictionary::~BSpellCheckerDictionary()
{
    //
}

/*============================== Public methods ============================*/

bool BSpellCheckerDictionary::spell(const QString &word) const
{
    if (!isValid())
        return false;
    return d_func()->hunspell->spell(codec()->fromUnicode(word).constData());
}

QStringList BSpellCheckerDictionary::suggest(const QString &word) const
{
    if (!isValid())
        return QStringList();
    char ***list = new char **;
    int count = d_func()->hunspell->suggest(list, d_func()->codec->fromUnicode(word).constData());
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
     d_func()->hunspell->add(codec()->fromUnicode(word).constData());
}

void BSpellCheckerDictionary::removeWord(const QString &word)
{
    if (!isValid())
        return;
     d_func()->hunspell->remove(codec()->fromUnicode(word).constData());
}

QString BSpellCheckerDictionary::path() const
{
    return d_func()->Path;
}

QLocale BSpellCheckerDictionary::locale() const
{
    return d_func()->Locale;
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
