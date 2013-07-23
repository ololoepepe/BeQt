#ifndef BLOCALDOCUMENTDRIVER_P_H
#define BLOCALDOCUMENTDRIVER_P_H

class BAbstractFileType;

class QTextCodec;

#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver_p.h"
#include "bextendedfiledialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BAbstractDocumentDriverPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
public:
    typedef BLocalDocumentDriver::Operation Operation;
public:
    struct LoadResult
    {
        Operation operation;
        bool success;
        QString text;
    };
    struct SaveResult
    {
        Operation operation;
        bool success;
    };
public:
    typedef QFuture<LoadResult> LoadResultFuture;
    typedef QFutureWatcher<LoadResult> LoadResultFutureWatcher;
    typedef QFuture<SaveResult> SaveResultFuture;
    typedef QFutureWatcher<SaveResult> SaveResultFutureWatcher;
public:
    static LoadResult loadFile(const Operation &op, const QString &fn, QTextCodec *codec);
    static SaveResult saveFile(const Operation &op, const QString &fn, QString txt, QTextCodec *codec,
                               bool native = false);
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
public:
    void init();
public Q_SLOTS:
    void loadOperationFinished();
    void saveOperationFinished();
public:
    QString defaultDir;
    bool nativeLineEnd;
    QByteArray fileDialogState;
    QByteArray fileDialogGeometry;
    BAbstractFileType *lastFileType;
    BTextCodecMenu::Style codecsComboBoxStyle;
private:
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
};

#endif // BLOCALDOCUMENTDRIVER_P_H
