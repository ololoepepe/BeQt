#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;
class BCoreApplicationPrivate;

class QLocale;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>
#include <QList>
#include <QString>

/*============================================================================
================================ Translator
============================================================================*/

class B_CORE_EXPORT BTranslator : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(QObject *parent = 0);
    explicit BTranslator(const QString &fileName, QObject *parent = 0);
    ~BTranslator();
    //
    void setFileName(const QString &fileName, bool languageChange = false);
    bool isValid() const;
    bool isInstalled() const;
    QString fileName() const;
    QList<QLocale> availableLocales() const;
protected:
    BTranslator(BTranslatorPrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BTranslator)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BTRANSLATOR_H
