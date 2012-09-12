#include "bnetworkoperation.h"
#include "bsocketwrapper.h"

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>

const BSocketWrapper::MetaData &BNetworkOperation::metaData() const
{
    return _m_CMetaData;
}

const QByteArray &BNetworkOperation::data() const
{
    return _m_data;
}

bool BNetworkOperation::isRequest() const
{
    return _m_CMetaData.isRequest();
}

bool BNetworkOperation::isValid() const
{
    return _m_CMetaData.isValid();
}

bool BNetworkOperation::isStarted() const
{
    return _m_isStarted;
}

bool BNetworkOperation::isError() const
{
    return _m_isError;
}

int BNetworkOperation::downloadProgress() const
{
    return _m_bytesInTotal > 0 ? ( (qreal) _m_bytesInReady / (qreal) _m_bytesInTotal ) * 100 : 100;
}

int BNetworkOperation::uploadProgress() const
{
    return _m_bytesOutTotal > 0 ? ( (qreal) _m_bytesOutReady / (qreal) _m_bytesOutTotal ) * 100 : 100;
}

bool BNetworkOperation::isFinished() const
{
    return _m_isFinished;
}

//

BNetworkOperation::BNetworkOperation(const BSocketWrapper::MetaData &metaData, QObject *parent) :
    QObject(parent), _m_CMetaData(metaData)
{
    _m_isStarted = false;
    _m_isError = false;
    _m_bytesInReady = 0;
    _m_bytesInTotal = -1;
    _m_bytesOutReady = 0;
    _m_bytesOutTotal = -1;
    _m_isFinished = false;
}

//

void BNetworkOperation::_m_setStarted()
{
    _m_isStarted = true;
    emit started();
}

void BNetworkOperation::_m_setError()
{
    _m_isError = true;
    emit error();
}

void BNetworkOperation::_m_setDownloadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    _m_bytesInReady = bytesReady;
    _m_bytesInTotal = bytesTotal;
    emit downloadProgress(bytesReady, bytesTotal);
}

void BNetworkOperation::_m_setUploadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    _m_bytesOutReady = bytesReady;
    _m_bytesOutTotal = bytesTotal;
    emit uploadProgress(bytesReady, bytesTotal);
}

void BNetworkOperation::_m_setFinished(const QByteArray &data)
{
    if ( !data.isEmpty() )
        _m_data = data;
    _m_isFinished = true;
    emit finished();
}
