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
================================ Person Info Provider
============================================================================*/

class BPersonInfoProvider : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BPersonInfoProvider)
    Q_OBJECT
    Q_DISABLE_COPY(BPersonInfoProvider)
public:
    struct PersonInfo
    {
        QString name;
        QString role;
        QString site;
        QString mail;
    };
    //
    typedef QList<PersonInfo> PersonInfoList;
    //
    explicit BPersonInfoProvider(QObject *parent = 0);
    explicit BPersonInfoProvider(const QString &fileName, QObject *parent = 0);
    ~BPersonInfoProvider();
protected:
    BPersonInfoProvider(BPersonInfoProviderPrivate &d, QObject *parent = 0);
public:
    void setFileName(const QString &fileName);
    QString fileName() const;
    PersonInfoList infos( const QLocale &locale = BCoreApplication::locale() ) const;
    PersonInfoList infos(const QString &localeName) const;
public slots:
    void reload();
signals:
    void reloaded();
};

#endif // BPERSONINFOPROVIDER_H

