#ifndef BNETWORKOPERATION_H
#define BNETWORKOPERATION_H

class BNetworkConnection;

#include "bsocketshell.h"

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>

#if defined(BNETWORK_LIBRARY)
#  define BNETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BNETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

class BNETWORKSHARED_EXPORT BNetworkOperation : public QObject
{
    Q_OBJECT
public:
    const QByteArray &data() const;
    const BSocketShell::MetaData &metaData() const;
    bool isRequest() const;
    bool isValid() const;
    bool isStarted() const;
    bool isError() const;
    int downloadProgress() const;
    int uploadProgress() const;
    bool isFinished() const;
private:
    friend class BNetworkConnection;
    //
    BSocketShell::MetaData _m_CMetaData;
    bool _m_isStarted;
    bool _m_isError;
    qint64 _m_bytesInReady;
    qint64 _m_bytesInTotal;
    qint64 _m_bytesOutReady;
    qint64 _m_bytesOutTotal;
    bool _m_isFinished;
    QByteArray _m_data;
    //
    explicit BNetworkOperation(const BSocketShell::MetaData &metaData, QObject *parent = 0);
    //
    void _m_setStarted();
    void _m_setError();
    void _m_setDownloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void _m_setUploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void _m_setFinished( const QByteArray &data = QByteArray() );
signals:
    void started();
    void error();
    void downloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void finished();
};

#endif // BNETWORKOPERATION_H
