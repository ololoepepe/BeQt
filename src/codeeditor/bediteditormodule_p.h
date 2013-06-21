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
================================ BEditEditorModulePrivate ====================
============================================================================*/

class B_CODEEDITOR_EXPORT BEditEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BEditEditorModule)
public:
    explicit BEditEditorModulePrivate(BEditEditorModule *q);
    ~BEditEditorModulePrivate();
public:
    void init();
    void setDocument(BCodeEditorDocument *doc);
    void checkActions();
    void checkSwitchModeAction();
    void resetSwitchModeAction(bool bm);
public Q_SLOTS:
    void retranslateUi();
    void actSwitchModeTriggered();
public:
    BCodeEditorDocument *document;
    QPointer<QAction> actCut;
    QPointer<QAction> actCopy;
    QPointer<QAction> actPaste;
    QPointer<QAction> actUndo;
    QPointer<QAction> actRedo;
    QPointer<QAction> actSwitchMode;
private:
    Q_DISABLE_COPY(BEditEditorModulePrivate)
};

#endif // BEDITEDITORMODULE_P_H
