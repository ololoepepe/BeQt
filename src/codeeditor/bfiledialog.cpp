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
    q->setOption(QFileDialog::DontUseNativeDialog);
    q->setFileMode(QFileDialog::AnyFile);
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
    if ( list.isEmpty() )
        return;
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

void BFileDialog::setSelectedCodec(QTextCodec *codec)
{
    if (!codec)
        return;
    B_D(BFileDialog);
    if ( !d->codecIndexes.contains(codec) )
        return;
    d->cmboxEncodings->setCurrentIndex( d->codecIndexes.value(codec) );
}

void BFileDialog::setSelectedCodec(const QString &codecName)
{
    if ( codecName.isEmpty() )
        return;
    setSelectedCodec( QTextCodec::codecForName( codecName.toLatin1() ) );
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
