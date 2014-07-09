/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bextendedfiledialog.h"

#include "babstractfiletype.h"

#include <BeQtCore/BeQt>
#include <BeQtWidgets/BFileDialog>
#include <BeQtWidgets/BTextCodecMenu>
#include <BeQtWidgets/private/bfiledialog_p.h>

#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialogPrivate : public BFileDialogPrivate
{
    Q_DECLARE_TR_FUNCTIONS(BExtendedFileDialogPrivate)
    B_DECLARE_PUBLIC(BExtendedFileDialog)
public:
    QList<BAbstractFileType *> fileTypes;
public:
    explicit BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle,
                                        const QString &topDir = QString());
    ~BExtendedFileDialogPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BExtendedFileDialogPrivate)
};

/*============================================================================
================================ BExtendedFileDialogPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BExtendedFileDialogPrivate::BExtendedFileDialogPrivate(BExtendedFileDialog *q, BTextCodecMenu::Style cmboxStyle,
                                                       const QString &topDir) :
    BFileDialogPrivate(q, cmboxStyle, topDir)
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

BExtendedFileDialog::BExtendedFileDialog(const QString &topDir, QWidget *parent) :
    BFileDialog(*new BExtendedFileDialogPrivate(this, BTextCodecMenu::StructuredStyle, topDir), parent)
{
    d_func()->init();
}

BExtendedFileDialog::BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent) :
    BFileDialog(*new BExtendedFileDialogPrivate(this, comboBoxStyle), parent)
{
    d_func()->init();
}

BExtendedFileDialog::BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent) :
    BFileDialog(*new BExtendedFileDialogPrivate(this, comboBoxStyle, topDir), parent)
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

void BExtendedFileDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    BFileDialog::restoreState(m.value("b_file_dialog_state").toByteArray());
    selectFileType(m.value("file_type_id").toString());
}

QByteArray BExtendedFileDialog::saveState() const
{
    QVariantMap m;
    m.insert("b_file_dialog_state", BFileDialog::saveState());
    m.insert("file_type_id", selectedFileTypeId());
    return BeQt::serialize(m);
}

BAbstractFileType *BExtendedFileDialog::selectedFileType() const
{
    QString sf = selectedNameFilter();
    if (sf.isEmpty())
        return 0;
    foreach (BAbstractFileType *ft, d_func()->fileTypes) {
        if (ft->createFileDialogFilter() == sf)
            return ft;
    }
    return 0;
}

QString BExtendedFileDialog::selectedFileTypeId() const
{
    BAbstractFileType *ft = selectedFileType();
    return ft ? ft->id() : QString();
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
    foreach (BAbstractFileType *ft, d_func()->fileTypes) {
        if (ft->id() == id)
            return selectNameFilter(ft->createFileDialogFilter());
    }
}

void BExtendedFileDialog::setFileTypes(const QList<BAbstractFileType *> &list)
{
    d_func()->fileTypes = list;
    if (list.isEmpty()) {
        setNameFilter("");
        return;
    }
    QStringList filters;
    foreach (BAbstractFileType *ft, list)
        filters += ft->createFileDialogFilter();
    setNameFilters(filters);
}
