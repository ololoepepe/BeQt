#ifndef BINDICATORSEDITORMODULE_P_H
#define BINDICATORSEDITORMODULE_P_H

#include "bindicatorseditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Indicators Editor Module Private
============================================================================*/

class BIndicatorsEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BIndicatorsEditorModule)
    B_DECLARE_TR_FUNCTIONS(BIndicatorsEditorModule, q)
    Q_OBJECT
public:
    explicit BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q);
    ~BIndicatorsEditorModulePrivate();
    //
    void init();
private:
    Q_DISABLE_COPY(BIndicatorsEditorModulePrivate)
};

#endif // BINDICATORSEDITORMODULE_P_H

