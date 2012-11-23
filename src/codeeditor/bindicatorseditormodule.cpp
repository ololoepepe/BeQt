#include "bindicatorseditormodule.h"
#include "bindicatorseditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"

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
    //
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

QString BIndicatorsEditorModulePrivate::createCursorPosIndicatorText()
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

void BIndicatorsEditorModulePrivate::updateCursorPosIndicators()
{
    QString text = createCursorPosIndicatorText();
    foreach (QLabel *lbl, cursorPosIndicators)
        lbl->setText(text);
}

//

void BIndicatorsEditorModulePrivate::retranslateUi()
{
    updateCursorPosIndicators();
}

void BIndicatorsEditorModulePrivate::cursorPosIndicatorDestroyed(QObject *obj)
{
    cursorPosIndicators.remove(obj);
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

QString BIndicatorsEditorModule::name() const
{
    return "beqt/indicators";
}

QWidget *BIndicatorsEditorModule::createIndicator(Indicator type, QWidget *parent)
{
    switch (type)
    {
    case CursorPositionIndicator:
        return d_func()->createCursorPosIndicator(parent);
    default:
        return 0;
    }
}

QList<QWidget *> BIndicatorsEditorModule::createIndicators(QWidget *parent)
{
    QList<QWidget *> list;
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
}

void BIndicatorsEditorModule::editorUnset(BCodeEditor *edr)
{
    d_func()->updateCursorPosIndicators();
}

void BIndicatorsEditorModule::documentCursorPositionChanged(const QPoint &pos)
{
    d_func()->updateCursorPosIndicators();
}

void BIndicatorsEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->updateCursorPosIndicators();
}
