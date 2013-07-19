#ifndef BPERSONINFOPROVIDER_H
#define BPERSONINFOPROVIDER_H

class BPersonInfoProviderPrivate;

class QString;

#include "bglobal.h"
#include "bbase.h"
#include "bpersoninfo.h"
#include "bcoreapplication.h"

#include <QObject>
#include <QLocale>
#include <QList>

/*============================================================================
================================BPersonInfoProvider ==========================
============================================================================*/

class B_CORE_EXPORT BPersonInfoProvider : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPersonInfoProvider)
public:
    static QString infoListToString(const BPersonInfoList &list);
    static QString infosString(const BPersonInfoProvider *prov, bool noDefault = false,
                               const QLocale &locale = BCoreApplication::locale());
    static QString infosString(const BPersonInfoProvider *prov, const QLocale &locale, bool noDefault = false);
public:
    explicit BPersonInfoProvider(QObject *parent = 0);
    explicit BPersonInfoProvider(const QString &fileName, QObject *parent = 0);
    ~BPersonInfoProvider();
protected:
    explicit BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent = 0);
public:
    void setFileName(const QString &fileName);
    QString fileName() const;
    BPersonInfoList infos(bool noDefault = false, const QLocale &locale = BCoreApplication::locale()) const;
    BPersonInfoList infos(const QLocale &locale, bool noDefault = false) const;
    QString infosString(bool noDefault = false, const QLocale &locale = BCoreApplication::locale()) const;
    QString infosString(const QLocale &locale, bool noDefault = false) const;
public Q_SLOTS:
    void reload();
Q_SIGNALS:
    void reloaded();
private:
    Q_DISABLE_COPY(BPersonInfoProvider)
};

#endif // BPERSONINFOPROVIDER_H
