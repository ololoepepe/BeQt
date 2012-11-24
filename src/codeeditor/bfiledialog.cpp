#include "bfiledialog.h"
#include "bfiledialog_p.h"
#include "babstractfiletype.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QFileDialog>
#include <QList>
#include <QString>
#include <QTextCodec>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

#include <QDebug>

/*============================================================================
================================ File Dialog Private
============================================================================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q) :
    BBasePrivate(q)
{
    //
}

BFileDialogPrivate::~BFileDialogPrivate()
{
    //
}

//

void BFileDialogPrivate::init()
{
    B_Q(BFileDialog);
    lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(trq("Encoding", "lbl text") + ":");
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

//

const QDataStream::Version BFileDialogPrivate::DSVersion = QDataStream::Qt_4_8;

/*============================================================================
================================ File Dialog
============================================================================*/

BFileDialog::BFileDialog(QWidget *parent) :
    QFileDialog(parent), BBase( *new BFileDialogPrivate(this) )
{
    d_func()->init();
}

BFileDialog::~BFileDialog()
{
    //
}

//

BFileDialog::BFileDialog(BFileDialogPrivate &d, QWidget *parent) :
    QFileDialog(parent), BBase(d)
{
    d_func()->init();
}

//

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
    QDataStream out(&ba, QIODevice::ReadOnly);
    out.setVersion(BFileDialogPrivate::DSVersion);
    out << QFileDialog::saveState();
    out << selectedCodecName();
    out << selectedFileTypeId();
    if (includeGeometry)
        out << saveGeometry();
    return ba;
}