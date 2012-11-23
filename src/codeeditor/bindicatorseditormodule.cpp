#include "bindicatorseditormodule.h"
#include "bindicatorseditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

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
    //
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

//

BIndicatorsEditorModule::BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

//

void BIndicatorsEditorModule::editorSet(BCodeEditor *edr)
{
    //
}

void BIndicatorsEditorModule::editorUnset(BCodeEditor *edr)
{
    //
}

void BIndicatorsEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    //
}
