#ifndef BABSTRACTEDITORMODULE_P_H
#define BABSTRACTEDITORMODULE_P_H

class BCodeEditor;
class BCodeEditorPrivate;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BAbstractEditorModulePrivate ================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractEditorModulePrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractEditorModule)
public:
    explicit BAbstractEditorModulePrivate(BAbstractEditorModule *q);
    ~BAbstractEditorModulePrivate();
public:
    void init();
    void setEditor(BCodeEditor *edr);
public:
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractEditorModulePrivate)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_P_H
