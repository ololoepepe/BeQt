#include "blocaldocumentdriver.h"
#include "blocaldocumentdriver_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"
#include "babstractfiletype.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>
#include <QWidget>
#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QList>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QVariant>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

#include <QDebug>

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

/*============================== Static public constants ===================*/

const QDataStream::Version BFileDialogPrivate::DSVersion = QDataStream::Qt_4_8;

/*============================== Public constructors =======================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q) :
    BBasePrivate(q)
{
    //
}

BFileDialogPrivate::~BFileDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BFileDialogPrivate::init()
{
    B_Q(BFileDialog);
    lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(lblEncodings);
      cmboxEncodings = new QComboBox(q);
        addEncoding( QTextCodec::codecForName("UTF-8") );
      lt->addWidget(cmboxEncodings);
}

void BFileDialogPrivate::addEncoding(QTextCodec *codec)
{
    if ( !codec || codecIndexes.contains(codec) )
        return;
    codecIndexes.insert( codec, cmboxEncodings->count() );
    QString fcn = BCodeEditor::fullCodecName(codec);
    QString cn = BCodeEditor::codecName(codec);
    cmboxEncodings->addItem(!fcn.isEmpty() ? fcn : cn, cn);
}

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialog::BFileDialog(QWidget *parent) :
    QFileDialog(parent), BBase( *new BFileDialogPrivate(this) )
{
    d_func()->init();
}

BFileDialog::~BFileDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BFileDialog::BFileDialog(BFileDialogPrivate &d, QWidget *parent) :
    QFileDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BFileDialog::setFileTypes(const QList<BAbstractFileType *> &list)
{
    d_func()->fileTypes = list;
    if ( list.isEmpty() )
    {
        setNameFilter("");
        return;
    }
    QStringList filters;
    foreach (BAbstractFileType *ft, list)
        filters += ft->createFileDialogFilter();
    setNameFilters(filters);
}

void BFileDialog::setCodecs(const QList<QTextCodec *> &list)
{
    if ( list.isEmpty() )
        return;
    B_D(BFileDialog);
    d->cmboxEncodings->clear();
    d->codecIndexes.clear();
    foreach (QTextCodec *c, list)
        d->addEncoding(c);
    selectCodec("UTF-8");
}

void BFileDialog::selectFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    selectFilter( ft->createFileDialogFilter() );
}

void BFileDialog::selectFileType(const QString &id)
{
    if ( id.isEmpty() )
        return;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->id() == id)
            return selectFilter( ft->createFileDialogFilter() );
}

void BFileDialog::selectCodec(QTextCodec *codec)
{
    if (!codec)
        return;
    B_D(BFileDialog);
    if ( !d->codecIndexes.contains(codec) )
        return;
    d->cmboxEncodings->setCurrentIndex( d->codecIndexes.value(codec) );
}

void BFileDialog::selectCodec(const QString &codecName)
{
    if ( codecName.isEmpty() )
        return;
    selectCodec( QTextCodec::codecForName( codecName.toLatin1() ) );
}

void BFileDialog::restoreState(const QByteArray &ba, bool includeGeometry)
{
    QDataStream in(ba);
    in.setVersion(BFileDialogPrivate::DSVersion);
    QByteArray fdstate;
    QString scn;
    QString sft;
    in >> fdstate;
    in >> scn;
    in >> sft;
    QFileDialog::restoreState(fdstate);
    selectCodec(scn);
    selectFileType(sft);
    if (includeGeometry)
    {
        QByteArray geom;
        in >> geom;
        restoreGeometry(geom);
    }
}

QTextCodec *BFileDialog::selectedCodec() const
{
    int ind = d_func()->cmboxEncodings->currentIndex();
    return (ind >= 0) ? QTextCodec::codecForName( selectedCodecName().toLatin1() ) : 0;
}

QString BFileDialog::selectedCodecName() const
{
    int ind = d_func()->cmboxEncodings->currentIndex();
    return (ind >= 0 ) ? d_func()->cmboxEncodings->itemData(ind).toString() : QString();
}

BAbstractFileType *BFileDialog::selectedFileType() const
{
    QString sf = selectedFilter();
    if ( sf.isEmpty() )
        return 0;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->createFileDialogFilter() == sf)
            return ft;
    return 0;
}

QString BFileDialog::selectedFileTypeId() const
{
    BAbstractFileType *ft = selectedFileType();
    return ft ? ft->id() : QString();
}

QByteArray BFileDialog::saveState(bool includeGeometry) const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BFileDialogPrivate::DSVersion);
    out << QFileDialog::saveState();
    out << selectedCodecName();
    out << selectedFileTypeId();
    if (includeGeometry)
        out << saveGeometry();
    return ba;
}

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
    BFileDialog bfd(parent);
    bfd.restoreState(d_func()->fileDialogState);
    if ( d_func()->fileDialogState.isEmpty() )
        bfd.setDirectory(d_func()->defaultDir);
    bfd.setFileTypes( editor()->fileTypes() );
    bfd.setCodecs( editor()->supportedCodecs() );
    if (codec)
        bfd.selectCodec(codec);
    bfd.setAcceptMode(BFileDialog::AcceptOpen);
    bfd.setFileMode(QFileDialog::ExistingFiles);
    int ret = bfd.exec();
    d_func()->fileDialogState = bfd.saveState();
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
    BFileDialog bfd(parent);
    bfd.restoreState(d_func()->fileDialogState);
    bfd.setFileTypes( editor()->fileTypes() );
    bfd.setCodecs( editor()->supportedCodecs() );
    bfd.selectCodec(codec);
    bfd.setAcceptMode(BFileDialog::AcceptSave);
    bfd.setFileMode(QFileDialog::AnyFile);
    QString dir = QFileInfo(fileName).path();
    if ( dir != "." && QDir(dir).exists() )
        bfd.setDirectory(dir); //TODO: Myabe should improve
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

void BLocalDocumentDriver::setDialogState(const QByteArray &state)
{
    d_func()->fileDialogState = state;
}

QByteArray BLocalDocumentDriver::dialogState() const
{
    return d_func()->fileDialogState;
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
