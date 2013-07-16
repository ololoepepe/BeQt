#include "bextendedfiledialog.h"
#include "babstractfiletype.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BTextCodecMenu>
#include <BeQtWidgets/BTextCodecComboBox>

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
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialogPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BExtendedFileDialogPrivate)
    B_DECLARE_PUBLIC(BExtendedFileDialog)
public:
    explicit BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle);
    ~BExtendedFileDialogPrivate();
public:
    void init();
public:
    const BTextCodecMenu::Style CmboxStyle;
public:
    QList<BAbstractFileType *> fileTypes;
    QLayout *lt;
      QLabel *lblEncodings;
      BTextCodecComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BExtendedFileDialogPrivate)
};

/*============================================================================
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BExtendedFileDialogPrivate::BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle) :
    BBasePrivate(q), CmboxStyle(cmboxStyle)
{
    //
}

BExtendedFileDialogPrivate::~BExtendedFileDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BExtendedFileDialogPrivate::init()
{
    B_Q(BExtendedFileDialog);
    q->setOption(BExtendedFileDialog::DontUseNativeDialog, true);
    lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(lblEncodings);
      cmboxEncodings = new BTextCodecComboBox(CmboxStyle);
      lt->addWidget(cmboxEncodings);
}

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

/*============================== Public constructors =======================*/

BExtendedFileDialog::BExtendedFileDialog(QWidget *parent) :
    QFileDialog(parent), BBase(*new BExtendedFileDialogPrivate(this, BTextCodecMenu::StructuredStyle))
{
    d_func()->init();
}

BExtendedFileDialog::BExtendedFileDialog(QWidget *parent, BTextCodecMenu::Style cmboxStyle) :
    QFileDialog(parent), BBase(*new BExtendedFileDialogPrivate(this, cmboxStyle))
{
    d_func()->init();
}

BExtendedFileDialog::~BExtendedFileDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BExtendedFileDialog::BExtendedFileDialog(BExtendedFileDialogPrivate &d, QWidget *parent) :
    QFileDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BExtendedFileDialog::setFileTypes(const QList<BAbstractFileType *> &list)
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

void BExtendedFileDialog::selectFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    selectNameFilter( ft->createFileDialogFilter() );
}

void BExtendedFileDialog::selectFileType(const QString &id)
{
    if ( id.isEmpty() )
        return;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->id() == id)
            return selectNameFilter( ft->createFileDialogFilter() );
}

void BExtendedFileDialog::setCodecSelectionEnabled(bool b)
{
    d_func()->cmboxEncodings->setVisible(b);
}

void BExtendedFileDialog::selectCodec(QTextCodec *codec)
{
    d_func()->cmboxEncodings->selectCodec(codec);
}

void BExtendedFileDialog::selectCodec(const QString &codecName)
{
    d_func()->cmboxEncodings->selectCodec(codecName);
}

void BExtendedFileDialog::restoreState(const QByteArray &ba, bool includeGeometry)
{
    QDataStream in(ba);
    in.setVersion(BeQt::DataStreamVersion);
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

BAbstractFileType *BExtendedFileDialog::selectedFileType() const
{
    QString sf = selectedNameFilter();
    if ( sf.isEmpty() )
        return 0;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->createFileDialogFilter() == sf)
            return ft;
    return 0;
}

QString BExtendedFileDialog::selectedFileTypeId() const
{
    BAbstractFileType *ft = selectedFileType();
    return ft ? ft->id() : QString();
}

bool BExtendedFileDialog::codecSelectionEnabled() const
{
    return d_func()->cmboxEncodings->isVisible();
}

QTextCodec *BExtendedFileDialog::selectedCodec() const
{

    return d_func()->cmboxEncodings->selectedCodec();
}

QString BExtendedFileDialog::selectedCodecName() const
{
    return d_func()->cmboxEncodings->selectedCodecName();
}

QByteArray BExtendedFileDialog::saveState(bool includeGeometry) const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BeQt::DataStreamVersion);
    out << QFileDialog::saveState();
    out << selectedCodecName();
    out << selectedFileTypeId();
    if (includeGeometry)
        out << saveGeometry();
    return ba;
}
