#ifndef BPERSONINFOPROVIDER_H
#define BPERSONINFOPROVIDER_H

class BPersonInfoProviderPrivate;

class QString;

#include "bglobal.h"
#include "bbase.h"
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
    struct PersonInfo
    {
        QString name;
        QString role;
        QString site;
        QString mail;
    };
public:
    typedef QList<PersonInfo> PersonInfoList;
public:
    explicit BPersonInfoProvider(QObject *parent = 0);
    explicit BPersonInfoProvider(const QString &fileName, QObject *parent = 0);
    ~BPersonInfoProvider();
protected:
    explicit BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent = 0);
public:
    void setFileName(const QString &fileName);
    QString fileName() const;
    PersonInfoList infos( bool noDefault = false, const QLocale &locale = BCoreApplication::locale() ) const;
    PersonInfoList infos(const QString &localeName, bool noDefault = false) const;
public slots:
    void reload();
signals:
    void reloaded();
private:
    Q_DISABLE_COPY(BPersonInfoProvider)
};

#endif // BPERSONINFOPROVIDER_H

