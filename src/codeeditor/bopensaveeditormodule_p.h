#ifndef BOPENSAVEEDITORMODULE_P_H
#define BOPENSAVEEDITORMODULE_P_H

#include "bopensaveeditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Open Save Editor Module Private
============================================================================*/

class BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BOpenSaveEditorModule)
    B_DECLARE_TR_FUNCTIONS(BOpenSaveEditorModule, q)
    Q_OBJECT
    Q_DISABLE_COPY(BOpenSaveEditorModulePrivate)
public:
    explicit BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q);
    ~BOpenSaveEditorModulePrivate();
    //
    void init();
};

#endif // BOPENSAVEEDITORMODULE_P_H

