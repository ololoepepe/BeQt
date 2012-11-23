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

void BFileDialogPrivate::addEncoding(QTextCodec *codec, BCodeEditor *editor)
{
    if (!codec )
        return;
    QString cn = QString::fromLatin1( codec->name().data() );
    if ( encodings.contains(cn) )
        return;
    encodings.insert(cn, codec);
    QString text = editor ? editor->fullCodecName(codec) : QString();
    cmboxEncodings->addItem(!text.isEmpty() ? text : cn, cn);
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

void BFileDialog::setEncodings(const QList<QTextCodec *> &list, BCodeEditor *editor)
{
    if ( list.isEmpty() )
        return;
    B_D(BFileDialog);
    d->cmboxEncodings->clear();
    d->encodings.clear();
    foreach (QTextCodec *c, list)
        d->addEncoding(c, editor);
}

void BFileDialog::setSelectedEncoding(QTextCodec *codec)
{
    if (!codec)
        return;
    setSelectedEncoding( QString::fromLatin1( codec->name().data() ) );
}

void BFileDialog::setSelectedEncoding(const QString &codecName)
{
    B_D(BFileDialog);
    int ind = d->cmboxEncodings->findData(codecName);
    if (ind < 0)
        return
    d->cmboxEncodings->setCurrentIndex(ind);
}

QTextCodec *BFileDialog::selectedEncoding() const
{
    QString scn = selectedCodecName();
    return !scn.isEmpty() ? d_func()->encodings.value(scn) : 0;
}

QString BFileDialog::selectedCodecName() const
{
    int ind = d_func()->cmboxEncodings->currentIndex();
    if (ind < 0)
        return "";
    return d_func()->cmboxEncodings->itemData(ind).toString();
}
