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

#include "bindicatorseditormodule.h"
#include "bindicatorseditormodule_p.h"

#include "babstractfiletype.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"

#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>
#include <BeQtWidgets/BGuiTools>

#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QWidget>

/*============================================================================
================================ BIndicatorsEditorModulePrivate ==============
============================================================================*/

/*============================== Public constructors =======================*/

BIndicatorsEditorModulePrivate::BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BIndicatorsEditorModulePrivate::~BIndicatorsEditorModulePrivate()
{
    if (!lblCursorPos->parent())
        delete lblCursorPos;
    if (!lblEncoding->parent())
        delete lblEncoding;
    if (!cmboxFileType->parent())
        delete cmboxFileType;
}

/*============================== Public methods ============================*/

void BIndicatorsEditorModulePrivate::init()
{
    lblCursorPos = new QLabel;
    lblCursorPos->setFont(BGuiTools::createMonospaceFont());
    updateCursorPosIndicator();
    lblEncoding = new QLabel;
    lblEncoding->setFont(BGuiTools::createMonospaceFont());
    updateEncodingIndicator();
    cmboxFileType = new QComboBox;
    cmboxFileType->setMinimumWidth(150);
    connect(cmboxFileType, SIGNAL(currentIndexChanged(int)), this, SLOT(cmboxFileTypeCurrentIndexChanged(int)));
    updateFileTypeIndicator();
    //
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BIndicatorsEditorModulePrivate::updateCursorPosIndicator()
{
    QPoint pos = q_func()->currentDocument() ? q_func()->currentDocument()->cursorPositionRowColumn() : QPoint(-1, -1);
    QString rowVal = (pos.x() >= 0) ? QString::number(pos.x() + 1) : QString();
    QString columnVal = (pos.y() >= 0) ? QString::number(pos.y() + 1) : QString();
    int len = qMax(rowVal.length(), columnVal.length());
    len = qMax(len, 4);
    if (rowVal.isEmpty() || columnVal.isEmpty()) {
        rowVal.fill('-', len);
        columnVal.fill('-', len);
    } else {
        if (rowVal.length() < len)
            rowVal.prepend(QString().fill(' ', len - rowVal.length()));
        if (columnVal.length() < len)
            columnVal.prepend(QString().fill(' ', len - columnVal.length()));
    }
    lblCursorPos->setText(tr("Column:", "lbl text") + " " + columnVal + ", " + tr("Row:", "lbl text") + " " + rowVal);
}

void BIndicatorsEditorModulePrivate::updateEncodingIndicator()
{
    if (!editor)
        return lblEncoding->setText("----------");
    QString cn = q_func()->currentDocument() ? q_func()->currentDocument()->codecName() : editor->defaultCodecName();
    QString fcn = BeQt::fullCodecName(cn);
    if ( fcn.isEmpty() )
        fcn = tr("Unknown encoding", "lbl text");
    lblEncoding->setText(fcn);
}

void BIndicatorsEditorModulePrivate::updateFileTypeIndicator()
{
    QList<FileTypeInfo> ftilist;
    int ind = -1;
    if (editor) {
        foreach (BAbstractFileType *ft, editor->fileTypes()) {
            FileTypeInfo fti;
            fti.id = ft->id();
            fti.name = ft->name();
            ftilist << fti;
        }
        if (q_func()->currentDocument()) {
            QString id = q_func()->currentDocument()->fileType()->id();
            QList<BAbstractFileType *> types = editor->fileTypes();
            for (int i = 0; i < types.size(); ++i) {
                if (types.at(i)->id() == id) {
                    ind = i;
                    break;
                }
            }
        }
    }
    cmboxFileType->blockSignals(true);
    cmboxFileType->clear();
    foreach (const FileTypeInfo &info, ftilist)
        cmboxFileType->addItem(info.name, info.id);
    cmboxFileType->setEnabled(ind >= 0);
    if (ind >= 0)
        cmboxFileType->setCurrentIndex(ind);
    cmboxFileType->blockSignals(false);
}

/*============================== Public slots ==============================*/

void BIndicatorsEditorModulePrivate::retranslateUi()
{
    updateCursorPosIndicator();
    updateEncodingIndicator();
    updateFileTypeIndicator();
}

void BIndicatorsEditorModulePrivate::cmboxFileTypeCurrentIndexChanged(int index)
{
    if (!editor || index < 0 || index >= cmboxFileType->count() || !q_func()->currentDocument())
        return;
    q_func()->currentDocument()->blockSignals(true);
    QString id = cmboxFileType->itemData(index).toString();
    foreach (BAbstractFileType *ft, editor->fileTypes()) {
        if (ft->id() == id) {
            q_func()->currentDocument()->setFileType(ft);
            q_func()->currentDocument()->setFocus();
            break;
        }
    }
    q_func()->currentDocument()->blockSignals(false);
}

/*============================================================================
================================ BIndicatorsEditorModule =====================
============================================================================*/

/*============================== Public constructors =======================*/

BIndicatorsEditorModule::BIndicatorsEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BIndicatorsEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BIndicatorsEditorModule::~BIndicatorsEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BIndicatorsEditorModule::BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BIndicatorsEditorModule::id() const
{
    return "beqt/indicators";
}

QWidget *BIndicatorsEditorModule::widget(int type)
{
    switch (type) {
    case CursorPositionIndicator:
        return d_func()->lblCursorPos;
    case EncodingIndicator:
        return d_func()->lblEncoding;
    case FileTypeIndicator:
        return d_func()->cmboxFileType;
    default:
        return 0;
    }
}

QList<QWidget *> BIndicatorsEditorModule::widgets(bool)
{
    B_D(BIndicatorsEditorModule);
    QList<QWidget *> list;
    list << d->lblCursorPos;
    list << d->lblEncoding;
    list << d->cmboxFileType;
    return list;
}

/*============================== Protected methods =========================*/

void BIndicatorsEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::defaultCodecChanged(const QString &)
{
    d_func()->updateEncodingIndicator();
}

void BIndicatorsEditorModule::documentCodecChanged(const QString &)
{
    d_func()->updateEncodingIndicator();
}

void BIndicatorsEditorModule::documentCursorPositionChanged(const QPoint &)
{
    d_func()->updateCursorPosIndicator();
}

void BIndicatorsEditorModule::documentFileTypeChanged(BAbstractFileType *)
{
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::editorSet(BCodeEditor *)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::editorUnset(BCodeEditor *)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::fileTypesChanged()
{
    d_func()->updateFileTypeIndicator();
}
