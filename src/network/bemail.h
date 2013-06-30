#ifndef BEMAIL_H
#define BEMAIL_H

class BEmailPrivate;

class QString;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QStringList>

/*============================================================================
================================ BEmail ======================================
============================================================================*/

class B_NETWORK_EXPORT BEmail : public BBase
{
    B_DECLARE_PRIVATE(BEmail)
public:
    explicit BEmail();
    BEmail(const BEmail &other);
    ~BEmail();
protected:
    explicit BEmail(BEmailPrivate &d);
public:
    void setSender(const QString &sender);
    void setReceiver(const QString &receiver);
    void setReceivers(const QStringList &list);
    void setSubject(const QString &subject);
    void setBody(const QString &body);
    QString sender() const;
    QString receiver(int index = 0) const;
    QStringList receivers() const;
    int receiversCount() const;
    QString subject() const;
    QString body() const;
    bool isValid() const;
public:
    BEmail &operator =(const BEmail &other);
    bool operator ==(const BEmail &other) const;
};

#endif // BEMAIL_H
