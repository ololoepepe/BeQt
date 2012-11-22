#ifndef BABSTRACTEDITORMODULE_P_H
#define BABSTRACTEDITORMODULE_P_H

class BCodeEditor;
class BCodeEditorPrivate;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Abstract Editor Module Private
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractEditorModulePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractEditorModule)
    B_DECLARE_TR_FUNCTIONS(BAbstractEditorModule, q)
    Q_OBJECT
public:
    explicit BAbstractEditorModulePrivate(BAbstractEditorModule *q);
    ~BAbstractEditorModulePrivate();
    //
    void init();
    void setEditor(BCodeEditor *edr);
    //
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractEditorModulePrivate)
    //
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_P_H

