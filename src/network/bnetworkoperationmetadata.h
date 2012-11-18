#ifndef BNETWORKOPERATIONMETADATA_H
#define BNETWORKOPERATIONMETADATA_H

class BNetworkOperationMetaDataPrivate;

class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QUuid>

/*============================================================================
================================ Network Operation Meta Data
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationMetaData : public BBase
{
    B_DECLARE_PRIVATE(BNetworkOperationMetaData)
public:
    BNetworkOperationMetaData();
    BNetworkOperationMetaData(const BNetworkOperationMetaData &other);
    BNetworkOperationMetaData(const QUuid &id, bool request, const QString &operation);
    //
    void setId(const QUuid &id);
    void setIsRequest(bool request);
    void setOperation(const QString &operation);
    void invalidate();
    const QUuid id() const;
    bool isRequest() const;
    const QString operation() const;
    bool isValid() const;
    //
    BNetworkOperationMetaData &operator=(const BNetworkOperationMetaData &other);
    bool operator==(const BNetworkOperationMetaData &other) const;
    bool operator<(const BNetworkOperationMetaData &other) const;
protected:
    BNetworkOperationMetaData(BNetworkOperationMetaDataPrivate &d);
};

#endif // BNETWORKOPERATIONMETADATA_H
