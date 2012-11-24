#ifndef BOPENSAVEEDITORMODULE_P_H
#define BOPENSAVEEDITORMODULE_P_H

class BCodeEditor;

class QAction;

#include "bopensaveeditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPointer>

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
    void checkActions();
public slots:
    void retranslateUi();
public:
    QPointer<QAction> actNewFile;
    QPointer<QAction> actOpenFiles;
    QPointer<QAction> actSaveFile;
    QPointer<QAction> actSaveFileAs;
    QPointer<QAction> actSaveAllFiles;
    QPointer<QAction> actCloseFile;
    QPointer<QAction> actCloseAllFiles;
};

#endif // BOPENSAVEEDITORMODULE_P_H

