#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Abstract Editor Module Private
============================================================================*/

BAbstractEditorModulePrivate::BAbstractEditorModulePrivate(BAbstractEditorModule *q) :
    BBasePrivate(q)
{
    //
}

BAbstractEditorModulePrivate::~BAbstractEditorModulePrivate()
{
    //
}

/*============================================================================
================================ Abstract Editor Module
============================================================================*/

BAbstractEditorModule::BAbstractEditorModule(QObject *parent) :
    QObject(parent), BBase( *new BAbstractEditorModulePrivate(this) )
{
    //
}

BAbstractEditorModule::~BAbstractEditorModule()
{
    //
}

//

BCodeEditor::EditorFeature BAbstractEditorModule::providesFeature() const
{
    return BCodeEditor::NoFeature;
}

//

BAbstractEditorModule::BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}

