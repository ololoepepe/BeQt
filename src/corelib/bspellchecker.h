#ifndef BSPELLCHECKER_H
#define BSPELLCHECKER_H

class BSpellCheckerPrivate;
class BSpellCheckerDictionary;

class QTextCodec;
class QLocale;

#include "bglobal.h"
#include "bbase.h"

#include <QStringList>
#include <QString>
#include <QList>

/*============================================================================
================================ BSpellChecker ===============================
============================================================================*/

class B_CORE_EXPORT BSpellChecker : public BBase
{
    B_DECLARE_PRIVATE(BSpellChecker)
public:
    explicit BSpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath = QString());
    explicit BSpellChecker(const QStringList &dictionaryPaths, const QString &userDictionaryPath = QString());
    ~BSpellChecker();
public:
    void addDictionary(const QString &path);
    void removeDictionary(const QLocale &locale);
    void removeDictionary(const QString &localeName);
    void setUserDictionary(const QString &path);
    QStringList dictionaryPaths() const;
    QString userDictionaryPath() const;
    bool spell(const QString &word) const;
    QStringList suggest(const QString &word) const;
    void ignoreWord(const QString &word, bool ignore = true);
    void ignoreWords(const QStringList &words, bool ignore = true);
    bool isIgnored(const QString &word) const;
    QList<BSpellCheckerDictionary *> dictionaries() const;
    BSpellCheckerDictionary *dictionary(const QLocale &locale) const;
    BSpellCheckerDictionary *dictionary(const QString &localeName) const;
private:
    Q_DISABLE_COPY(BSpellChecker)
};

#endif // BSPELLCHECKER_H
