#ifndef BEDITEDITORMODULE_P_H
#define BEDITEDITORMODULE_P_H

class BCodeEditorDocument;

class QAction;

#include "bediteditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QList>
#include <QPointer>

/*============================================================================
================================ Edit Editor Module Private
============================================================================*/

class BEditEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BEditEditorModule)
    B_DECLARE_TR_FUNCTIONS(BEditEditorModule, q)
    Q_OBJECT
    Q_DISABLE_COPY(BEditEditorModulePrivate)
public:
    explicit BEditEditorModulePrivate(BEditEditorModule *q);
    ~BEditEditorModulePrivate();
    //
    void init();
    void setDocument(BCodeEditorDocument *doc);
    void checkActions();
public slots:
    void retranslateUi();
public:
    BCodeEditorDocument *document;
    QPointer<QAction> actCut;
    QPointer<QAction> actCopy;
    QPointer<QAction> actPaste;
    QPointer<QAction> actUndo;
    QPointer<QAction> actRedo;
};

#endif // BEDITEDITORMODULE_P_H

