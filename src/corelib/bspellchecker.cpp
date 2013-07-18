#include "bspellchecker.h"
#include "bspellchecker_p.h"
#include "bspellcheckerdictionary.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bnamespace.h"
#include "bdirtools.h"

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
#include <QDir>
#include <QUuid>
#include <QtGlobal>
#include <QRegExp>
#include <QList>
#include <QObject>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#endif

#include <QDebug>

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

/*============================== Static public methods =====================*/

void BSpellCheckerPrivate::flush(const QString &fileName, const QStringList &words)
{
    BDirTools::writeTextFile(fileName, words.join("\n"), "UTF-8");
}

/*============================== Public constructors =======================*/

BSpellCheckerPrivate::BSpellCheckerPrivate(BSpellChecker *q) :
    BBasePrivate(q)
{
    //
}

BSpellCheckerPrivate::~BSpellCheckerPrivate()
{
    if (userDictPath.isEmpty())
        return;
    QStringList words = ignored.toList();
    words.removeAll("");
    flush(userDictPath, words);
}

/*============================== Public methods ============================*/

void BSpellCheckerPrivate::init()
{
    considerLeft = -1;
    considerRight = -1;
}

/*============================== Public slots ==============================*/

void BSpellCheckerPrivate::flush()
{
    if (userDictPath.isEmpty())
        return;
    QStringList words = ignored.toList();
    words.removeAll("");
    QtConcurrent::run(&BSpellCheckerPrivate::flush, userDictPath, words);
    QTimer::singleShot(BeQt::Minute, this, SLOT(flush()));
}

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

/*============================== Public constructors =======================*/

BSpellChecker::BSpellChecker(QObject *parent) :
    QObject(parent), BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
}

BSpellChecker::BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath) :
    QObject(0), BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    addDictionary(dictionaryPath);
    setUserDictionary(userDictionaryPath);
}

BSpellChecker::BSpellChecker(QObject *parent, const QString &dictionaryPath, const QString &userDictionaryPath) :
    QObject(parent), BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    addDictionary(dictionaryPath);
    setUserDictionary(userDictionaryPath);
}

BSpellChecker::BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath) :
    QObject(0), BBase(*new BSpellCheckerPrivate(this))
{
    d_func()->init();
    foreach (const QString &path, dictionaryPaths)
        addDictionary(path);
    setUserDictionary(userDictionaryPath);
}

BSpellChecker::BSpellChecker(QObject *parent, const QStringList &dictionaryPaths, const QString &userDictionaryPath) :
    QObject(parent), BBase(*new BSpellCheckerPrivate(this))
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
    QString npath;
    if (path.startsWith(":/"))
    {
        npath = QDir::tempPath() + "/beqt/spellchecker/" + BeQt::pureUuidText(QUuid::createUuid()) + "/" + fn;
        if (!BDirTools::copyDir(path, npath))
        {
            BDirTools::rmdir(npath);
            return;
        }
    }
    BSpellCheckerDictionary *dict = new BSpellCheckerDictionary(npath.isEmpty() ? path : npath);
    if (!npath.isEmpty())
        BDirTools::rmdir(QFileInfo(npath).path());
    if (!dict->isValid())
    {
        delete dict;
        return;
    }
    d_func()->dicts.insert(fn, dict);
    Q_EMIT changed();
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
    Q_EMIT changed();
}

void BSpellChecker::setUserDictionary(const QString &path)
{
    if (d_func()->userDictPath == path)
        return;
    d_func()->ignored.clear();
    d_func()->userDictPath.clear();
    if (path.isEmpty())
    {
        Q_EMIT changed();
        return;
    }
    bool ok = false;
    ignoreWords(BDirTools::readTextFile(path, "UTF-8", &ok).split('\n'));
    if (!ok && !BDirTools::touch(path))
    {
        Q_EMIT changed();
        return;
    }
    d_func()->userDictPath = path;
    QTimer::singleShot(d_func()->ignored.size() * 100 + BeQt::Minute, d_func(), SLOT(flush()));
    Q_EMIT changed();
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

bool BSpellChecker::spell(const QString &word, const QString &surrLeft, const QString &surrRight) const
{
    if (word.isEmpty())
        return false;
    QString w = word;
    if (d_func()->considerLeft)
        w.prepend(surrLeft.right(d_func()->considerLeft));
    if (d_func()->considerRight)
        w.append(surrRight.left(d_func()->considerRight));
    foreach (const QRegExp &rx, d_func()->ignoredRx4)
        if (rx.exactMatch(w))
            return true;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    foreach (const QRegularExpression &rx, d_func()->ignoredRx5)
        if (rx.match(w).hasMatch())
            return true;
#endif
    if (d_func()->ignored.contains(word) || d_func()->ignoredImplicitly.contains(word))
        return true;
    foreach (BSpellCheckerDictionary *dict, d_func()->dicts)
        if (dict->spell(word))
            return true;
    return false;
}

QStringList BSpellChecker::suggest(const QString &word) const
{
    if (word.isEmpty())
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
    foreach (const QString &w, words)
    {
        if (ignore)
            d_func()->ignored.insert(w);
        else
            d_func()->ignored.remove(w);
    }
    if (!words.isEmpty())
        Q_EMIT changed();
}

void BSpellChecker::ignoreWordImplicitly(const QString &word, bool ignore)
{
    ignoreWordsImplicitly(QStringList() << word, ignore);
}

void BSpellChecker::ignoreWordsImplicitly(const QStringList &words, bool ignore)
{
    foreach (const QString &w, words)
    {
        if (ignore)
            d_func()->ignoredImplicitly.insert(w);
        else
            d_func()->ignoredImplicitly.remove(w);
    }
    if (!words.isEmpty())
        Q_EMIT changed();
}

void BSpellChecker::ignoreImplicitlyRegExp(const QRegExp &rx, bool ignore)
{
    if (ignore)
        d_func()->ignoredRx4 << rx;
    else
        d_func()->ignoredRx4.removeAll(rx);
    Q_EMIT changed();
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void BSpellChecker::ignoreImplicitlyRegExp(const QRegularExpression &rx, bool ignore)
{
    if (ignore)
    {
        d_func()->ignoredRx5 << rx;
        QRegularExpression &rxx = d_func()->ignoredRx5.last();
        QString pattern = rxx.pattern();
        if (!pattern.startsWith('^'))
            pattern.prepend('^');
        if (!pattern.endsWith('$'))
            pattern.append('$');
    }
    else
    {
        d_func()->ignoredRx5.removeAll(rx);
    }
    Q_EMIT changed();
}
#endif

void BSpellChecker::considerLeftSurrounding(int charCount)
{
    if (charCount < 0)
        charCount = -1;
    if (charCount == d_func()->considerLeft)
        return;
    d_func()->considerLeft = charCount;
    Q_EMIT changed();
}

void BSpellChecker::considerRightSurrounding(int charCount)
{
    if (charCount < 0)
        charCount = -1;
    if (charCount == d_func()->considerRight)
        return;
    d_func()->considerRight = charCount;
    Q_EMIT changed();
}

void BSpellChecker::clearIgnored()
{
    bool b = !d_func()->ignored.isEmpty();
    d_func()->ignored.clear();
    if (b)
        Q_EMIT changed();
}

void BSpellChecker::clearIgnoredImplicitly()
{
    bool b = !d_func()->ignoredImplicitly.isEmpty();
    d_func()->ignoredImplicitly.clear();
    if (b)
        Q_EMIT changed();
}

void BSpellChecker::clearIgnoredImplicitlyRegExp()
{
    bool b = !d_func()->ignoredRx4.isEmpty();
    d_func()->ignoredRx4.clear();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    b = b || d_func()->ignoredRx5.isEmpty();
    d_func()->ignoredRx5.clear();
#endif
    if (b)
        Q_EMIT changed();
}

bool BSpellChecker::isIgnored(const QString &word, bool *implicitly) const
{
    return isIgnored(implicitly, word);
}

bool BSpellChecker::isIgnored(bool *implicitly, const QString &word, const QString &surrLeft,
                              const QString &surrRight) const
{
    if (word.isEmpty())
        return false;
    QString w = word;
    if (d_func()->considerLeft)
        w.prepend(surrLeft.right(d_func()->considerLeft));
    if (d_func()->considerRight)
        w.append(surrRight.left(d_func()->considerRight));
    foreach (const QRegExp &rx, d_func()->ignoredRx4)
    {
        if (rx.exactMatch(w))
            return bRet(implicitly, true, true);
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    foreach (const QRegularExpression &rx, d_func()->ignoredRx5)
        if (rx.match(w).hasMatch())
            return bRet(implicitly, true, true);
#endif
    if (d_func()->ignored.contains(word))
        return bRet(implicitly, false, true);
    else if (d_func()->ignoredImplicitly.contains(word))
        return bRet(implicitly, true, true);
    else
        return false;
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
