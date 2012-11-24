#include "bopensaveeditormodule.h"
#include "bopensaveeditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ Open Save Editor Module Private
============================================================================*/

BOpenSaveEditorModulePrivate::BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BOpenSaveEditorModulePrivate::~BOpenSaveEditorModulePrivate()
{
    //
}

//

void BOpenSaveEditorModulePrivate::init()
{
    //
}

/*============================================================================
================================ Open Save Editor Module
============================================================================*/

BOpenSaveEditorModule::BOpenSaveEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BOpenSaveEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BOpenSaveEditorModule::~BOpenSaveEditorModule()
{
    //
}

//

BOpenSaveEditorModule::BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

//

QString BOpenSaveEditorModule::id() const
{
    return "beqt/open_save";
}

//

void BOpenSaveEditorModule::editorSet(BCodeEditor *edr)
{
    //
}

void BOpenSaveEditorModule::editorUnset(BCodeEditor *edr)
{
    //
}

void BOpenSaveEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    //
}
