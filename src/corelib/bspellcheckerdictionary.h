#ifndef BSPELLCHECKERDICTIONARY_H
#define BSPELLCHECKERDICTIONARY_H

class BSpellCheckerDictionaryPrivate;

class QString;
class QTextCodec;
class QLocale;

#include "bglobal.h"
#include "bbase.h"

#include <QStringList>

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

class B_CORE_EXPORT BSpellCheckerDictionary : public BBase
{
    B_DECLARE_PRIVATE(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionary(const QString &path);
    ~BSpellCheckerDictionary();
public:
    bool spell(const QString &word) const;
    QStringList suggest(const QString &word) const;
    void addWord(const QString &word);
    void removeWord(const QString &word);
    QString path() const;
    QLocale locale() const;
    QTextCodec *codec() const;
    QString codecName() const;
    bool isValid() const;
private:
    Q_DISABLE_COPY(BSpellCheckerDictionary)
};

#endif // BSPELLCHECKERDICTIONARY_H
