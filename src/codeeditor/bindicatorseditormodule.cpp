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
    foreach (QLabel *lbl, cursorPosIndicators)
        if ( !lbl->parent() )
            lbl->deleteLater();
    foreach (QLabel *lbl, encodingIndicators)
        if ( !lbl->parent() )
            lbl->deleteLater();
    foreach (QComboBox *cmbox, fileTypeIndicators)
        if ( !cmbox->parent() )
            cmbox->deleteLater();
}

//

void BIndicatorsEditorModulePrivate::init()
{
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

QLabel *BIndicatorsEditorModulePrivate::createCursorPosIndicator(QWidget *parent)
{
    QLabel *lbl = new QLabel(parent);
    lbl->setFont( BApplication::createMonospaceFont() );
    lbl->setText( createCursorPosIndicatorText() );
    connect( lbl, SIGNAL( destroyed(QObject *) ), this, SLOT( cursorPosIndicatorDestroyed(QObject *) ) );
    cursorPosIndicators.insert(lbl, lbl);
    return lbl;
}

QLabel *BIndicatorsEditorModulePrivate::createEncodingIndicator(QWidget *parent)
{
    QLabel *lbl = new QLabel(parent);
    lbl->setFont( BApplication::createMonospaceFont() );
    lbl->setText( createEncodingIndicatorText() );
    connect( lbl, SIGNAL( destroyed(QObject *) ), this, SLOT( encodingIndicatorDestroyed(QObject *) ) );
    encodingIndicators.insert(lbl, lbl);
    return lbl;
}

QComboBox *BIndicatorsEditorModulePrivate::createFileTypeIndicator(QWidget *parent)
{
    QComboBox *cmbox = new QComboBox(parent);
    cmbox->setMinimumWidth(150);
    processFileTypeIndicator( cmbox, createFileTypeInfos(), createFileTypeIndicatorIndex() );
    connect( cmbox, SIGNAL( destroyed(QObject *) ), this, SLOT( fileTypeIndicatorDestroyed(QObject *) ) );
    connect( cmbox, SIGNAL( currentIndexChanged(int) ), this, SLOT( cmboxCurrentIndexChanged(int) ) );
    fileTypeIndicators.insert(cmbox, cmbox);
    return cmbox;
}

QString BIndicatorsEditorModulePrivate::createCursorPosIndicatorText() const
{
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
    return trq("Row:", "lbl text") + " " + rowVal + ", " + trq("Column:", "lbl text") + " " + columnVal;
}

QString BIndicatorsEditorModulePrivate::createEncodingIndicatorText() const
{
    if (!editor)
        return trq("----------", "lbl text");
    QString cn = q_func()->currentDocument() ? q_func()->currentDocument()->codecName() : editor->defaultCodecName();
    QString fcn = editor->fullCodecName(cn);
    if ( fcn.isEmpty() )
        fcn = trq("Unknown encoding", "lbl text");
    return fcn;
}

QList<BIndicatorsEditorModulePrivate::FileTypeInfo> BIndicatorsEditorModulePrivate::createFileTypeInfos() const
{
    if (!editor)
        return QList<FileTypeInfo>();
    QList<FileTypeInfo> list;
    foreach ( BAbstractFileType *ft, editor->fileTypes() )
    {
        FileTypeInfo fti;
        fti.id = ft->id();
        fti.name = ft->name();
        list << fti;
    }
    return list;
}

int BIndicatorsEditorModulePrivate::createFileTypeIndicatorIndex() const
{
    if ( !editor || !q_func()->currentDocument() )
        return -1;
    QString id = q_func()->currentDocument()->fileType()->id();
    QList<BAbstractFileType *> types = editor->fileTypes();
    for (int i = 0; i < types.size(); ++i)
        if (types.at(i)->id() == id)
            return i;
    return -1;
}

void BIndicatorsEditorModulePrivate::processFileTypeIndicator(QComboBox *cmbox, const QList<FileTypeInfo> &infos,
                                                              int index)
{
    cmbox->blockSignals(true);
    cmbox->clear();
    foreach (const FileTypeInfo &info, infos)
        cmbox->addItem(info.name, info.id);
    cmbox->setEnabled(index >= 0);
    if (index >= 0)
        cmbox->setCurrentIndex(index);
    cmbox->blockSignals(false);
}

void BIndicatorsEditorModulePrivate::updateCursorPosIndicators()
{
    QString text = createCursorPosIndicatorText();
    foreach (QLabel *lbl, cursorPosIndicators)
        lbl->setText(text);
}

void BIndicatorsEditorModulePrivate::updateEncodingIndicators()
{
    QString text = createEncodingIndicatorText();
    foreach (QLabel *lbl, encodingIndicators)
        lbl->setText(text);
}

void BIndicatorsEditorModulePrivate::updateFileTypeIndicators()
{
    QList<FileTypeInfo> list = createFileTypeInfos();
    int index = createFileTypeIndicatorIndex();
    foreach (QComboBox *cmbox, fileTypeIndicators)
        processFileTypeIndicator(cmbox, list, index);
}

//

void BIndicatorsEditorModulePrivate::retranslateUi()
{
    updateCursorPosIndicators();
    updateEncodingIndicators();
    updateFileTypeIndicators();
}

void BIndicatorsEditorModulePrivate::cursorPosIndicatorDestroyed(QObject *obj)
{
    cursorPosIndicators.remove(obj);
}

void BIndicatorsEditorModulePrivate::encodingIndicatorDestroyed(QObject *obj)
{
    encodingIndicators.remove(obj);
}

void BIndicatorsEditorModulePrivate::fileTypeIndicatorDestroyed(QObject *obj)
{
    fileTypeIndicators.remove(obj);
}

void BIndicatorsEditorModulePrivate::cmboxCurrentIndexChanged(int index)
{
    QComboBox *cmbox = static_cast<QComboBox *>( sender() );
    if ( !editor || !cmbox || index < 0 || index >= cmbox->count() || !q_func()->currentDocument() )
        return;
    q_func()->currentDocument()->blockSignals(true);
    QString id = cmbox->itemData(index).toString();
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

QWidget *BIndicatorsEditorModule::createIndicator(Indicator type, QWidget *parent)
{
    switch (type)
    {
    case CursorPositionIndicator:
        return d_func()->createCursorPosIndicator(parent);
    case EncodingIndicator:
        return d_func()->createEncodingIndicator(parent);
    case FileTypeIndicator:
        return d_func()->createFileTypeIndicator(parent);
    default:
        return 0;
    }
}

QList<QWidget *> BIndicatorsEditorModule::createIndicators(QWidget *parent)
{
    QList<QWidget *> list;
    list << d_func()->createCursorPosIndicator(parent);
    list << d_func()->createEncodingIndicator(parent);
    list << d_func()->createFileTypeIndicator(parent);
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
    d_func()->updateCursorPosIndicators();
    d_func()->updateEncodingIndicators();
    d_func()->updateFileTypeIndicators();
}

void BIndicatorsEditorModule::editorUnset(BCodeEditor *edr)
{
    d_func()->updateCursorPosIndicators();
    d_func()->updateEncodingIndicators();
    d_func()->updateFileTypeIndicators();
}

void BIndicatorsEditorModule::documentCursorPositionChanged(const QPoint &pos)
{
    d_func()->updateCursorPosIndicators();
}

void BIndicatorsEditorModule::documentCodecChanged(const QString &codecName)
{
    d_func()->updateEncodingIndicators();
}

void BIndicatorsEditorModule::documentFileTypeChanged(BAbstractFileType *ft)
{
    d_func()->updateFileTypeIndicators();
}

void BIndicatorsEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->updateCursorPosIndicators();
    d_func()->updateEncodingIndicators();
    d_func()->updateFileTypeIndicators();
}

void BIndicatorsEditorModule::fileTypesChanged()
{
    d_func()->updateFileTypeIndicators();
}
