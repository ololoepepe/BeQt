#ifndef BNETWORKOPERATIONMETADATA_H
#define BNETWORKOPERATIONMETADATA_H

class BNetworkOperationMetaDataPrivate;

class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QUuid>

/*============================================================================
================================ BNetworkOperationMetaData ===================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationMetaData : public BBase
{
    B_DECLARE_PRIVATE(BNetworkOperationMetaData)
public:
    explicit BNetworkOperationMetaData();
    BNetworkOperationMetaData(const BNetworkOperationMetaData &other); //Should not be explicit
    explicit BNetworkOperationMetaData(const QUuid &id, bool request, const QString &operation);
protected:
    explicit BNetworkOperationMetaData(BNetworkOperationMetaDataPrivate &d);
public:
    void setId(const QUuid &id);
    void setIsRequest(bool request);
    void setOperation(const QString &operation);
    void invalidate();
    const QUuid id() const;
    bool isRequest() const;
    const QString operation() const;
    bool isValid() const;
public:
    BNetworkOperationMetaData &operator=(const BNetworkOperationMetaData &other);
    bool operator==(const BNetworkOperationMetaData &other) const;
    bool operator<(const BNetworkOperationMetaData &other) const;
};

#endif // BNETWORKOPERATIONMETADATA_H
