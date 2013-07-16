#include "bextendedfiledialog.h"
#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>
#include <BeQtWidgets/BTextCodecMenu>
#include <BeQtWidgets/BFileDialog>
#include <BeQtWidgets/private/bfiledialog_p.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QByteArray>

#include <QDebug>

/*============================================================================
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialogPrivate : public BFileDialogPrivate
{
    Q_DECLARE_TR_FUNCTIONS(BExtendedFileDialogPrivate)
    B_DECLARE_PUBLIC(BExtendedFileDialog)
public:
    explicit BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle);
    ~BExtendedFileDialogPrivate();
public:
    void init();
public:
    QList<BAbstractFileType *> fileTypes;
private:
    Q_DISABLE_COPY(BExtendedFileDialogPrivate)
};

/*============================================================================
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BExtendedFileDialogPrivate::BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle) :
    BFileDialogPrivate(q, cmboxStyle)
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
    //
}

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

/*============================== Public constructors =======================*/

BExtendedFileDialog::BExtendedFileDialog(QWidget *parent) :
    BFileDialog(*new BExtendedFileDialogPrivate(this, BTextCodecMenu::StructuredStyle), parent)
{
    d_func()->init();
}

BExtendedFileDialog::BExtendedFileDialog(BTextCodecMenu::Style cmboxStyle, QWidget *parent) :
    BFileDialog(*new BExtendedFileDialogPrivate(this, cmboxStyle), parent)
{
    d_func()->init();
}

BExtendedFileDialog::~BExtendedFileDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BExtendedFileDialog::BExtendedFileDialog(BExtendedFileDialogPrivate &d, QWidget *parent) :
    BFileDialog(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BExtendedFileDialog::setFileTypes(const QList<BAbstractFileType *> &list)
{
    d_func()->fileTypes = list;
    if (list.isEmpty())
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
    selectNameFilter(ft->createFileDialogFilter());
}

void BExtendedFileDialog::selectFileType(const QString &id)
{
    if (id.isEmpty())
        return;
    foreach (BAbstractFileType *ft, d_func()->fileTypes)
        if (ft->id() == id)
            return selectNameFilter(ft->createFileDialogFilter());
}

void BExtendedFileDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    BFileDialog::restoreState(m.value("b_file_dialog_state").toByteArray());
    selectFileType(m.value("file_type_id").toString());
}

BAbstractFileType *BExtendedFileDialog::selectedFileType() const
{
    QString sf = selectedNameFilter();
    if (sf.isEmpty())
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

QByteArray BExtendedFileDialog::saveState() const
{
    QVariantMap m;
    m.insert("b_file_dialog_state", BFileDialog::saveState());
    m.insert("file_type_id", selectedFileTypeId());
    return BeQt::serialize(m);
}
