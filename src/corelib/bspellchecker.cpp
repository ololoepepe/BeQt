#include "bspellchecker.h"
#include "bspellcheckerdictionary.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QFileInfo>
#include <QSet>
#include <QFile>
#include <QTextStream>
#include <QtConcurrentRun>
#include <QTimer>

#include <QDebug>

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

class BSpellCheckerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSpellChecker)
public:
    static void flush(const QString &fileName, const QStringList &words);
public:
    explicit BSpellCheckerPrivate(BSpellChecker *q);
    ~BSpellCheckerPrivate();
public:
    void init();
public Q_SLOTS:
    void flush();
public:
    QMap<QString, BSpellCheckerDictionary *> dicts;
    QString userDictPath;
    QSet<QString> ignored;
    QStringList ignoredList;
private:
    Q_DISABLE_COPY(BSpellCheckerPrivate)
};

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

/*============================== Static public methods =====================*/

void BSpellCheckerPrivate::flush(const QString &fileName, const QStringList &words)
{
    QFile f(fileName);
    if (!f.open(QFile::WriteOnly | QFile::Truncate))
        return;
    QTextStream out(&f);
    foreach (const QString &w, words)
        out << w << "\n";
    f.close();
}

/*============================== Public constructors =======================*/

BSpellCheckerPrivate::BSpellCheckerPrivate(BSpellChecker *q) :
    BBasePrivate(q)
{
    //
}

BSpellCheckerPrivate::~BSpellCheckerPrivate()
{
    if (!userDictPath.isEmpty())
        flush(userDictPath, ignoredList);
}

/*============================== Public methods ============================*/

void BSpellCheckerPrivate::init()
{
    //
}

/*============================== Public slots ==============================*/

void BSpellCheckerPrivate::flush()
{
    if (!userDictPath.isEmpty())
    {
        QtConcurrent::run(&BSpellCheckerPrivate::flush, userDictPath, ignoredList);
        QTimer::singleShot(BeQt::Minute, this, SLOT(flush()));
    }
}

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

/*============================== Public constructors =======================*/

BSpellChecker::BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath) :
    BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    addDictionary(dictionaryPath);
    setUserDictionary(userDictionaryPath);
}

BSpellChecker::BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath) :
    BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    foreach (const QString &path, dictionaryPaths)
        addDictionary(path);
    setUserDictionary(userDictionaryPath);
}

BSpellChecker::~BSpellChecker()
{
    //
}

/*============================== Public methods ============================*/

void BSpellChecker::addDictionary(const QString &path)
{
    QString fn = QFileInfo(path).fileName();
    if (path.isEmpty() || d_func()->dicts.contains(fn))
        return;
    BSpellCheckerDictionary *dict = new BSpellCheckerDictionary(path);
    if (!dict->isValid())
    {
        delete dict;
        return;
    }
    d_func()->dicts.insert(fn, dict);
}

void BSpellChecker::removeDictionary(const QLocale &locale)
{
    removeDictionary(locale.name());
}

void BSpellChecker::removeDictionary(const QString &localeName)
{
    if (localeName.isEmpty())
        return;
    delete d_func()->dicts.take(localeName);
}

void BSpellChecker::setUserDictionary(const QString &path)
{
    if (d_func()->userDictPath == path)
        return;
    d_func()->ignored.clear();
    d_func()->userDictPath.clear();
    if (path.isEmpty())
        return;
    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return;
    d_func()->userDictPath = path;
    QTextStream in(&f);
    QStringList sl;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.isEmpty())
            sl << line;
    }
    f.close();
    ignoreWords(sl);
    QTimer::singleShot(d_func()->ignoredList.size() * 100, d_func(), SLOT(flush()));
}

QStringList BSpellChecker::dictionaryPaths() const
{
    QStringList sl;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        sl << dict->path();
    return sl;
}

QString BSpellChecker::userDictionaryPath() const
{
    return d_func()->userDictPath;
}

bool BSpellChecker::spell(const QString &word) const
{
    if (word.isEmpty() || !isValid())
        return false;
    if (d_func()->ignored.contains(word))
        return true;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        if (dict->spell(word))
            return true;
    return false;
}

QStringList BSpellChecker::suggest(const QString &word) const
{
    if (word.isEmpty() || !isValid())
        return QStringList();
    QStringList sl;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        sl << dict->suggest(word);
    return sl;
}

void BSpellChecker::ignoreWord(const QString &word, bool ignore)
{
    ignoreWords(QStringList() << word, ignore);
}

void BSpellChecker::ignoreWords(const QStringList &words, bool ignore)
{
    if (!isValid())
        return;
    foreach (const QString &w, words)
    {
        if (ignore)
        {
            if (d_func()->ignored.contains(w))
                return;
            d_func()->ignored.insert(w);
            d_func()->ignoredList << w;
        }
        else
        {
            if (!d_func()->ignored.contains(w))
                return;
            d_func()->ignored.remove(w);
            d_func()->ignoredList.removeAll(w);
        }
    }
}

QList<BSpellCheckerDictionary *> BSpellChecker::dictionaries() const
{
    return d_func()->dicts.values();
}

BSpellCheckerDictionary *BSpellChecker::dictionary(const QLocale &locale) const
{
    return dictionary(locale.name());
}

BSpellCheckerDictionary *BSpellChecker::dictionary(const QString &localeName) const
{
    return d_func()->dicts.value(localeName);
}

bool BSpellChecker::isValid() const
{
    return !d_func()->dicts.isEmpty();
}
