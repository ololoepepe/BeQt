#ifndef BSPELLCHECKER_H
#define BSPELLCHECKER_H

class BSpellCheckerPrivate;
class BSpellCheckerDictionary;

class QTextCodec;
class QLocale;
class QRegExp;

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
class QRegularExpression;
#endif

#include "bglobal.h"
#include "bbase.h"

#include <QStringList>
#include <QString>
#include <QList>
#include <QObject>

/*============================================================================
================================ BSpellChecker ===============================
============================================================================*/

class B_CORE_EXPORT BSpellChecker : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSpellChecker)
public:
    explicit BSpellChecker(QObject *parent = 0);
    explicit BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath = QString());
    explicit BSpellChecker(QObject *parent, const QString &dictionaryPath,
                           const QString &userDictionaryPath = QString());
    explicit BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath = QString());
    explicit BSpellChecker(QObject *parent, const QStringList &dictionaryPaths,
                           const QString &userDictionaryPath = QString());
    ~BSpellChecker();
public:
    void addDictionary(const QString &path);
    void removeDictionary(const QLocale &locale);
    void removeDictionary(const QString &localeName);
    void setUserDictionary(const QString &path);
    QStringList dictionaryPaths() const;
    QString userDictionaryPath() const;
    bool spell(const QString &word, const QString &surrLeft = QString(), const QString &surrRight = QString()) const;
    QStringList suggest(const QString &word) const;
    void ignoreWord(const QString &word, bool ignore = true);
    void ignoreWords(const QStringList &words, bool ignore = true);
    void ignoreWordImplicitly(const QString &word, bool ignore = true);
    void ignoreWordsImplicitly(const QStringList &words, bool ignore = true);
    void ignoreImplicitlyRegExp(const QRegExp &rx, bool ignore = true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void ignoreImplicitlyRegExp(const QRegularExpression &rx, bool ignore = true);
#endif
    void considerLeftSurrounding(int charCount);
    void considerRightSurrounding(int charCount);
    void clearIgnored();
    void clearIgnoredImplicitly();
    void clearIgnoredImplicitlyRegExp();
    bool isIgnored(const QString &word, bool *implicitly = 0) const;
    bool isIgnored(bool *implicitly, const QString &word, const QString &surrLeft = QString(),
                   const QString &surrRight = QString()) const;
    QList<BSpellCheckerDictionary *> dictionaries() const;
    BSpellCheckerDictionary *dictionary(const QLocale &locale) const;
    BSpellCheckerDictionary *dictionary(const QString &localeName) const;
Q_SIGNALS:
    void changed();
private:
    Q_DISABLE_COPY(BSpellChecker)
};

#endif // BSPELLCHECKER_H
