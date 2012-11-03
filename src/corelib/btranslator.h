#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;
class BCoreApplicationPrivate;

class QLocale;

#include "bnamespace.h"

#include <QObject>
#include <QList>
#include <QString>

class B_CORE_EXPORT BTranslator : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
        explicit BTranslator(const QString &fileName = QString(), QObject *parent = 0);
    ~BTranslator();
    //
    void setFileName(const QString &fileName, bool languageChange = false);
    bool isValid() const;
    bool isInstalled() const;
    QString fileName() const;
    QList<QLocale> availableLocales() const;
protected:
    BTranslatorPrivate *_m_d;
private:
    Q_DISABLE_COPY(BTranslator)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BTRANSLATOR_H
