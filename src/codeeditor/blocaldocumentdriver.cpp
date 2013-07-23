#include "blocaldocumentdriver.h"
#include "blocaldocumentdriver_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"
#include "babstractfiletype.h"
#include "bextendedfiledialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>
#include <QWidget>
#include <QStringList>
#include <QDir>
#include <QList>
#include <QVariant>
#include <QByteArray>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QVariantMap>

#include <QDebug>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

/*============================== Static public methods =====================*/

BLocalDocumentDriverPrivate::LoadResult BLocalDocumentDriverPrivate::loadFile(const Operation &op, const QString &fn,
                                                                              QTextCodec *codec)
{
    QFile f(fn);
    LoadResult res;
    res.operation = op;
    if (!f.open(QFile::ReadOnly))
    {
        res.success = false;
        return res;
    }
    QTextStream in(&f);
    in.setCodec(codec);
    res.text = in.readAll();
    f.close();
    res.success = true;
    return res;
}

BLocalDocumentDriverPrivate::SaveResult BLocalDocumentDriverPrivate::saveFile(const Operation &op, const QString &fn,
                                                                              QString txt, QTextCodec *codec,
                                                                              bool native)
{
    Q_UNUSED(native);
    QFile f(fn);
    SaveResult res;
    res.operation = op;
    if (!f.open(QFile::WriteOnly))
    {
        res.success = false;
        return res;
    }
    QTextStream out(&f);
    out.setCodec(codec);
#if defined(Q_OS_WIN)
    if (native)
        txt.replace('\n', "\r\n");
#endif
    out << txt;
    f.close();
    res.success = true;
    return res;
}

/*============================== Public constructors =======================*/

BLocalDocumentDriverPrivate::BLocalDocumentDriverPrivate(BLocalDocumentDriver *q) :
    BAbstractDocumentDriverPrivate(q)
{
    //
}

BLocalDocumentDriverPrivate::~BLocalDocumentDriverPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLocalDocumentDriverPrivate::init()
{
    defaultDir = QDir::homePath();
    nativeLineEnd = true;
    lastFileType = 0;
    codecsComboBoxStyle = BTextCodecMenu::StructuredStyle;
}

/*============================== Public slots ==============================*/

void BLocalDocumentDriverPrivate::loadOperationFinished()
{
    LoadResultFutureWatcher *watcher = dynamic_cast<LoadResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    LoadResult res = watcher->result();
    delete watcher;
    q_func()->emitLoadingFinished(res.operation, res.success, res.text);
}

void BLocalDocumentDriverPrivate::saveOperationFinished()
{
    SaveResultFutureWatcher *watcher = dynamic_cast<SaveResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    SaveResult res = watcher->result();
    delete watcher;
    q_func()->emitSavingFinished(res.operation, res.success);
}

/*============================================================================
================================ BLocalDocumentDriver ========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalDocumentDriver::BLocalDocumentDriver(QObject *parent) :
    BAbstractDocumentDriver(*new BLocalDocumentDriverPrivate(this), parent)
{
    d_func()->init();
}

BLocalDocumentDriver::~BLocalDocumentDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BLocalDocumentDriver::BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent) :
    BAbstractDocumentDriver(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BLocalDocumentDriver::id() const
{
    return "beqt/local";
}

bool BLocalDocumentDriver::isBuisy() const
{
    return false;
}

bool BLocalDocumentDriver::testFileExistance(const QString &fileName)
{
    return QFileInfo(fileName).isFile();
}

bool BLocalDocumentDriver::testFileReadOnly(const QString &fileName)
{
    QFile f(fileName);
    bool b = !f.open(QFile::WriteOnly | QFile::Append);
    f.close();
    return b;
}

bool BLocalDocumentDriver::getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec)
{
    if (!editor())
        return false;
    B_D(BLocalDocumentDriver);
    BExtendedFileDialog bfd(d->codecsComboBoxStyle, parent);
    bfd.restoreState(d->fileDialogState);
    if (!bfd.restoreGeometry(d->fileDialogGeometry))
        bfd.resize(700, 400);
    if (d->fileDialogState.isEmpty())
        bfd.setDirectory(d->defaultDir);
    bfd.setFileTypes(editor()->fileTypes());
    bfd.selectFileType(d->lastFileType ? d->lastFileType : editor()->preferredFileType());
    if (codec)
        bfd.selectCodec(codec);
    bfd.setAcceptMode(BExtendedFileDialog::AcceptOpen);
    bfd.setFileMode(QFileDialog::ExistingFiles);
    int ret = bfd.exec();
    d->fileDialogState = bfd.saveState();
    d->fileDialogGeometry = bfd.saveGeometry();
    d->lastFileType = bfd.selectedFileType();
    if (BExtendedFileDialog::Accepted != ret)
        return false;
    fileNames = bfd.selectedFiles();
    codec = bfd.selectedCodec();
    return true;
}

bool BLocalDocumentDriver::getSaveAsFileName(QWidget *parent, const QString &fileName,
                                             QString &newName, QTextCodec *&codec)
{
    if (!editor())
        return false;
    B_D(BLocalDocumentDriver);
    BExtendedFileDialog bfd(d->codecsComboBoxStyle, parent);
    bfd.restoreState(d->fileDialogState);
    if (!bfd.restoreGeometry(d->fileDialogGeometry))
        bfd.resize(700, 400);
    bfd.setFileTypes(editor()->fileTypes());
    bfd.selectCodec(codec);
    bfd.setAcceptMode(BExtendedFileDialog::AcceptSave);
    bfd.setFileMode(QFileDialog::AnyFile);
    QString dir = QFileInfo(fileName).path();
    if (dir != "." && QDir(dir).exists())
        bfd.setDirectory(dir); //TODO: Myabe should improve (needs testing)
    else if (d->fileDialogState.isEmpty())
        bfd.setDirectory(d->defaultDir);
    bfd.selectFile(fileName);
    int ret = bfd.exec();
    d->fileDialogState = bfd.saveState();
    d->fileDialogGeometry = bfd.saveGeometry();
    if (BExtendedFileDialog::Accepted != ret)
        return false;
    newName = bfd.selectedFiles().first();
    codec = bfd.selectedCodec();
    return true;
}

QByteArray BLocalDocumentDriver::saveState() const
{
    QVariantMap m;
    m.insert("file_dialog_state", d_func()->fileDialogState);
    m.insert("file_dialog_geometry", d_func()->fileDialogGeometry);
    return BeQt::serialize(m);
}

void BLocalDocumentDriver::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    d_func()->fileDialogState = m.value("file_dialog_state").toByteArray();
    d_func()->fileDialogGeometry = m.value("file_dialog_geometry").toByteArray();
}

void BLocalDocumentDriver::setDefaultDir(const QString &dir)
{
    if ( dir.isEmpty() )
        return;
    d_func()->defaultDir = dir;
}

void BLocalDocumentDriver::setNativeLineEnd(bool enabled)
{
    d_func()->nativeLineEnd = enabled;
}

void BLocalDocumentDriver::setCodecsComboBoxStyle(BTextCodecMenu::Style style)
{
    d_func()->codecsComboBoxStyle = style;
}

/*============================== Protected methods =========================*/

bool BLocalDocumentDriver::handleLoadOperation(const Operation &op)
{
    QString fn = !op.fileName.isEmpty() ? op.fileName : op.document->fileName();
    QTextCodec *c = op.codec ? op.codec : op.document->codec();
    BLocalDocumentDriverPrivate::LoadResultFuture future =
            QtConcurrent::run(&BLocalDocumentDriverPrivate::loadFile, op, fn, c);
    BLocalDocumentDriverPrivate::LoadResultFutureWatcher *watcher =
            new BLocalDocumentDriverPrivate::LoadResultFutureWatcher(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), d_func(), SLOT(loadOperationFinished()));
    return true;
}

bool BLocalDocumentDriver::handleSaveOperation(const Operation &op)
{
    QString fn = !op.fileName.isEmpty() ? op.fileName : op.document->fileName();
    QTextCodec *c = op.codec ? op.codec : op.document->codec();
    QString txt = op.document->text();
    BLocalDocumentDriverPrivate::SaveResultFuture future =
            QtConcurrent::run(&BLocalDocumentDriverPrivate::saveFile, op, fn, txt, c, d_func()->nativeLineEnd);
    BLocalDocumentDriverPrivate::SaveResultFutureWatcher *watcher =
            new BLocalDocumentDriverPrivate::SaveResultFutureWatcher(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), d_func(), SLOT(saveOperationFinished()));
    return true;
}
