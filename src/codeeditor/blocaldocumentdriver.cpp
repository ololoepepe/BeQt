#include "blocaldocumentdriver.h"
#include "blocaldocumentdriver_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"
#include "babstractfiletype.h"
#include "bfiledialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

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

#include <QDebug>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

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
    codecsCmboxStyle = BFileDialog::StructuredStyle;
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
    if ( !editor() )
        return false;
    B_D(BLocalDocumentDriver);
    BFileDialog bfd(parent, d->codecsCmboxStyle);
    bfd.restoreState(d->fileDialogState);
    if ( d->fileDialogState.isEmpty() )
        bfd.setDirectory(d->defaultDir);
    bfd.setFileTypes( editor()->fileTypes() );
    bfd.selectFileType( d->lastFileType ? d->lastFileType : editor()->preferredFileType() );
    if (codec)
        bfd.selectCodec(codec);
    bfd.setAcceptMode(BFileDialog::AcceptOpen);
    bfd.setFileMode(QFileDialog::ExistingFiles);
    int ret = bfd.exec();
    d->fileDialogState = bfd.saveState();
    d->lastFileType = bfd.selectedFileType();
    if (BFileDialog::Accepted != ret)
        return false;
    fileNames = bfd.selectedFiles();
    codec = bfd.selectedCodec();
    return true;
}

bool BLocalDocumentDriver::getSaveAsFileName(QWidget *parent, const QString &fileName,
                                             QString &newName, QTextCodec *&codec)
{
    if ( !editor() )
        return false;
    BFileDialog bfd(parent, d_func()->codecsCmboxStyle);
    bfd.restoreState(d_func()->fileDialogState);
    bfd.setFileTypes( editor()->fileTypes() );
    bfd.selectCodec(codec);
    bfd.setAcceptMode(BFileDialog::AcceptSave);
    bfd.setFileMode(QFileDialog::AnyFile);
    QString dir = QFileInfo(fileName).path();
    if ( dir != "." && QDir(dir).exists() )
        bfd.setDirectory(dir); //TODO (needs testing): Myabe should improve
    else if ( d_func()->fileDialogState.isEmpty() )
        bfd.setDirectory(d_func()->defaultDir);
    bfd.selectFile(fileName);
    int ret = bfd.exec();
    d_func()->fileDialogState = bfd.saveState();
    if (BFileDialog::Accepted != ret)
        return false;
    newName = bfd.selectedFiles().first();
    codec = bfd.selectedCodec();
    return true;
}

QByteArray BLocalDocumentDriver::saveState() const
{
    return d_func()->fileDialogState;
}

void BLocalDocumentDriver::restoreState(const QByteArray &state)
{
    d_func()->fileDialogState = state;
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

void BLocalDocumentDriver::setCodecsComboBoxStyle(BFileDialog::CodecsComboBoxStyle style)
{
    d_func()->codecsCmboxStyle = style;
}

/*============================== Protected methods =========================*/

bool BLocalDocumentDriver::handleLoadOperation(const Operation &op)
{
    QFile f( !op.fileName.isEmpty() ? op.fileName : op.document->fileName() );
    if ( !f.open(QFile::ReadOnly) )
    {
        emitLoadingFinished(op, false);
        return false;
    }
    QTextStream in(&f);
    in.setCodec( op.codec ? op.codec : op.document->codec() );
    QString text = in.readAll();
    f.close();
    emitLoadingFinished(op, true, text);
    return true;
}

bool BLocalDocumentDriver::handleSaveOperation(const Operation &op)
{
    QFile f( !op.fileName.isEmpty() ? op.fileName : op.document->fileName() );
    if ( !f.open(QFile::WriteOnly) )
    {
        emitSavingFinished(op, false);
        return false;
    }
    QTextStream out(&f);
    out.setCodec( op.codec ? op.codec : op.document->codec() );
    QString text = op.document->text();
#if defined(Q_OS_WIN)
    if (d_func()->nativeLineEnd)
        text.replace('\n', "\r\n");
#endif
    out << text;
    f.close();
    emitSavingFinished(op, true);
    return true;
}
