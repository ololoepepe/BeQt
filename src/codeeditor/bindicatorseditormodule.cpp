#include "bindicatorseditormodule.h"
#include "bindicatorseditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"
#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QString>
#include <QWidget>
#include <QList>
#include <QLabel>
#include <QPoint>
#include <QFont>
#include <QComboBox>
#include <QStringList>
#include <QVariant>

#include <QDebug>

/*============================================================================
================================ Indicators Editor Module Private
============================================================================*/

BIndicatorsEditorModulePrivate::BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BIndicatorsEditorModulePrivate::~BIndicatorsEditorModulePrivate()
{
    if ( !lblCursorPos.isNull() && !lblCursorPos->parent() )
        lblCursorPos->deleteLater();
    if ( !lblEncoding.isNull() && !lblEncoding->parent() )
        lblEncoding->deleteLater();
    if ( !cmboxFileType.isNull() && !cmboxFileType->parent() )
        cmboxFileType->deleteLater();
}

//

void BIndicatorsEditorModulePrivate::init()
{
    lblCursorPos = new QLabel;
    lblCursorPos->setFont( BApplication::createMonospaceFont() );
    updateCursorPosIndicator();
    lblEncoding = new QLabel;
    lblEncoding->setFont( BApplication::createMonospaceFont() );
    updateEncodingIndicator();
    cmboxFileType = new QComboBox;
    cmboxFileType->setMinimumWidth(150);
    connect( cmboxFileType.data(), SIGNAL( currentIndexChanged(int) ),
             this, SLOT( cmboxFileTypeCurrentIndexChanged(int) ) );
    updateFileTypeIndicator();
    //
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BIndicatorsEditorModulePrivate::updateCursorPosIndicator()
{
    if ( lblCursorPos.isNull() )
        return;
    QPoint pos = q_func()->currentDocument() ? q_func()->currentDocument()->cursorPosition() : QPoint(-1, -1);
    QString rowVal = (pos.y() >= 0) ? QString::number(pos.y() + 1) : QString();
    QString columnVal = (pos.x() >= 0) ? QString::number(pos.x() + 1) : QString();
    int len = qMax( rowVal.length(), columnVal.length() );
    len = qMax(len, 4);
    if ( rowVal.isEmpty() || columnVal.isEmpty() )
    {
        rowVal.fill('-', len);
        columnVal.fill('-', len);
    }
    else
    {
        if (rowVal.length() < len)
            rowVal.prepend( QString().fill( ' ', len - rowVal.length() ) );
        if (columnVal.length() < len)
            columnVal.prepend( QString().fill( ' ', len - columnVal.length() ) );
    }
    lblCursorPos->setText(trq("Row:", "lbl text") + " " + rowVal + ", " +
                          trq("Column:", "lbl text") + " " + columnVal);
}

void BIndicatorsEditorModulePrivate::updateEncodingIndicator()
{
    if ( lblEncoding.isNull() )
        return;
    if (!editor)
        return lblEncoding->setText( trq("----------", "lbl text") );
    QString cn = q_func()->currentDocument() ? q_func()->currentDocument()->codecName() : editor->defaultCodecName();
    QString fcn = editor->fullCodecName(cn);
    if ( fcn.isEmpty() )
        fcn = trq("Unknown encoding", "lbl text");
    lblEncoding->setText(fcn);
}

void BIndicatorsEditorModulePrivate::updateFileTypeIndicator()
{
    if ( cmboxFileType.isNull() )
        return;
    QList<FileTypeInfo> ftilist;
    int ind = -1;
    if (editor)
    {
        foreach ( BAbstractFileType *ft, editor->fileTypes() )
        {
            FileTypeInfo fti;
            fti.id = ft->id();
            fti.name = ft->name();
            ftilist << fti;
        }
        if ( q_func()->currentDocument() )
        {
            QString id = q_func()->currentDocument()->fileType()->id();
            QList<BAbstractFileType *> types = editor->fileTypes();
            for (int i = 0; i < types.size(); ++i)
            {
                if (types.at(i)->id() == id)
                {
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

//

void BIndicatorsEditorModulePrivate::retranslateUi()
{
    updateCursorPosIndicator();
    updateEncodingIndicator();
    updateFileTypeIndicator();
}

void BIndicatorsEditorModulePrivate::cmboxFileTypeCurrentIndexChanged(int index)
{
    if ( !editor || cmboxFileType.isNull() || index < 0 || index >= cmboxFileType->count() ||
         !q_func()->currentDocument() )
        return;
    q_func()->currentDocument()->blockSignals(true);
    QString id = cmboxFileType->itemData(index).toString();
    foreach ( BAbstractFileType *ft, editor->fileTypes() )
    {
        if (ft->id() == id)
        {
            q_func()->currentDocument()->setFileType(ft);
            q_func()->currentDocument()->setFocus();
            break;
        }
    }
    q_func()->currentDocument()->blockSignals(false);
}

/*============================================================================
================================ Indicators Editor Module
============================================================================*/

BIndicatorsEditorModule::BIndicatorsEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BIndicatorsEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BIndicatorsEditorModule::~BIndicatorsEditorModule()
{
    //
}

//

QString BIndicatorsEditorModule::id() const
{
    return "beqt/indicators";
}

QWidget *BIndicatorsEditorModule::indicator(Indicator type)
{
    switch (type)
    {
    case CursorPositionIndicator:
        return d_func()->lblCursorPos.data();
    case EncodingIndicator:
        return d_func()->lblEncoding.data();
    case FileTypeIndicator:
        return d_func()->cmboxFileType.data();
    default:
        return 0;
    }
}

QList<QWidget *> BIndicatorsEditorModule::indicators()
{
    B_D(BIndicatorsEditorModule);
    QList<QWidget *> list;
    if ( !d->lblCursorPos.isNull() )
        list << d->lblCursorPos.data();
    if ( !d->lblEncoding.isNull() )
        list << d->lblEncoding.data();
    if ( !d->cmboxFileType.isNull() )
        list << d->cmboxFileType.data();
    return list;
}

//

BIndicatorsEditorModule::BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

//

void BIndicatorsEditorModule::editorSet(BCodeEditor *edr)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::editorUnset(BCodeEditor *edr)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::documentCursorPositionChanged(const QPoint &pos)
{
    d_func()->updateCursorPosIndicator();
}

void BIndicatorsEditorModule::documentCodecChanged(const QString &codecName)
{
    d_func()->updateEncodingIndicator();
}

void BIndicatorsEditorModule::documentFileTypeChanged(BAbstractFileType *ft)
{
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::defaultCodecChanged(const QString &codecName)
{
    d_func()->updateEncodingIndicator();
}

void BIndicatorsEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->updateCursorPosIndicator();
    d_func()->updateEncodingIndicator();
    d_func()->updateFileTypeIndicator();
}

void BIndicatorsEditorModule::fileTypesChanged()
{
    d_func()->updateFileTypeIndicator();
}
