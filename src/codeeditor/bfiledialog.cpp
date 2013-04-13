#include "bfiledialog.h"
#include "babstractfiletype.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QStringList>
#include <QFileDialog>
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

class B_CODEEDITOR_EXPORT BFileDialogPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BFileDialogPrivate)
    B_DECLARE_PUBLIC(BFileDialog)
public:
    static const QDataStream::Version DSVersion;
public:
    explicit BFileDialogPrivate(BFileDialog *q, BFileDialog::CodecsComboBoxStyle cmboxStyle);
    ~BFileDialogPrivate();
public:
    void init();
public:
    const BFileDialog::CodecsComboBoxStyle CmboxStyle;
public:
    QList<BAbstractFileType *> fileTypes;
    QLayout *lt;
      QLabel *lblEncodings;
      QComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

/*============================== Static public constants ===================*/

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    const QDataStream::Version BFileDialogPrivate::DSVersion = QDataStream::Qt_4_8;
#else
    const QDataStream::Version BFileDialogPrivate::DSVersion = QDataStream::Qt_5_0;
#endif

/*============================== Public constructors =======================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q, BFileDialog::CodecsComboBoxStyle cmboxStyle) :
    BBasePrivate(q), CmboxStyle(cmboxStyle)
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
    q->setOption(BFileDialog::DontUseNativeDialog, true);
    lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(lblEncodings);
      cmboxEncodings = (CmboxStyle == BFileDialog::PlainStyle) ? BCodeEditor::createPlainCodecsComboBox(q) :
                                                                 BCodeEditor::createStructuredCodecsComboBox(q);
      lt->addWidget(cmboxEncodings);
}

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialog::BFileDialog(QWidget *parent) :
    QFileDialog(parent), BBase( *new BFileDialogPrivate(this, StructuredStyle) )
{
    d_func()->init();
}

BFileDialog::BFileDialog(QWidget *parent, CodecsComboBoxStyle cmboxStyle) :
    QFileDialog(parent), BBase( *new BFileDialogPrivate(this, cmboxStyle) )
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

void BFileDialog::selectFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    selectNameFilter( ft->createFileDialogFilter() );
}

void BFileDialog::selectFileType(const QString &id)
{
    if ( id.isEmpty() )
        return;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->id() == id)
            return selectNameFilter( ft->createFileDialogFilter() );
}

void BFileDialog::setCodecSelectionEnabled(bool b)
{
    d_func()->cmboxEncodings->setVisible(b);
}

void BFileDialog::selectCodec(QTextCodec *codec)
{
    BCodeEditor::selectCodec(d_func()->cmboxEncodings, codec);
}

void BFileDialog::selectCodec(const QString &codecName)
{
    BCodeEditor::selectCodec(d_func()->cmboxEncodings, codecName);
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
    QStringList h = history();
    if (h.size() > 20)
        setHistory( h.mid(h.size() - 20, 20) ); //Truncate long history
    selectCodec(scn);
    selectFileType(sft);
    if (includeGeometry)
    {
        QByteArray geom;
        in >> geom;
        restoreGeometry(geom);
    }
}

BAbstractFileType *BFileDialog::selectedFileType() const
{
    QString sf = selectedNameFilter();
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

bool BFileDialog::codecSelectionEnabled() const
{
    return d_func()->cmboxEncodings->isVisible();
}

QTextCodec *BFileDialog::selectedCodec() const
{
    return BCodeEditor::selectedCodec(d_func()->cmboxEncodings);
}

QString BFileDialog::selectedCodecName() const
{
    return BCodeEditor::selectedCodecName(d_func()->cmboxEncodings);
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
