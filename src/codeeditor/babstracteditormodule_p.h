#ifndef BABSTRACTEDITORMODULE_P_H
#define BABSTRACTEDITORMODULE_P_H

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Abstract Editor Module Private
============================================================================*/

class BAbstractEditorModulePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractEditorModule)
    B_DECLARE_TR_FUNCTIONS(BAbstractEditorModule, q)
    Q_OBJECT
public:
    explicit BAbstractEditorModulePrivate(BAbstractEditorModule *q);
    ~BAbstractEditorModulePrivate();
    //
private:
    Q_DISABLE_COPY(BAbstractEditorModulePrivate)
};

#endif // BABSTRACTEDITORMODULE_P_H

