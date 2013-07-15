#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;
class BCoreApplicationPrivate;

class QLocale;
class QTranslator;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>
#include <QList>
#include <QString>

/*============================================================================
================================ BTranslator =================================
============================================================================*/

class B_CORE_EXPORT BTranslator : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(QObject *parent = 0);
    explicit BTranslator(const QString &fileName, QObject *parent = 0);
    explicit BTranslator(const QLocale &locale, const QString &fileName, QObject *parent = 0);
    ~BTranslator();
protected:
    explicit BTranslator(BTranslatorPrivate &d, QObject *parent = 0);
public:
    void setFileName(const QString &fileName);
    void setLocale(const QLocale &locale);
    bool load(const QString &fileName = QString());
    bool load(const QLocale &locale, const QString &fileName = QString());
    void unload();
    bool isValid() const;
    bool isLoaded() const;
    QString fileName() const;
    QLocale locale() const;
    QList<QLocale> availableLocales() const;
    QList<QTranslator *> translators() const;
    QString translate(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) const;
private:
    Q_DISABLE_COPY(BTranslator)
    friend class BCoreApplicationPrivate;
};

#endif // BTRANSLATOR_H
