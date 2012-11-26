#ifndef BOPENSAVEEDITORMODULE_P_H
#define BOPENSAVEEDITORMODULE_P_H

class BCodeEditor;

class QAction;
class QString;
class QMenu;

#include "bopensaveeditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPointer>
#include <QList>
#include <QStringList>

/*============================================================================
================================ Open Save Editor Module Private
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
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
    void resetFileHistory( const QStringList &list = QStringList() );
public slots:
    void retranslateUi();
    void codecTriggered();
    void fileTriggered();
public:
    QPointer<QAction> actNewFile;
    QPointer<QAction> actOpenFiles;
    QPointer<QAction> actReopenFile;
    QPointer<QAction> actSaveFile;
    QPointer<QAction> actSaveFileAs;
    QPointer<QAction> actSaveAllFiles;
    QPointer<QAction> actCloseFile;
    QPointer<QAction> actCloseAllFiles;
    QList<QMenu *> codecGroups;
    QList<QAction *> codecs;
    QPointer<QMenu> mnuFileHistory;
};

#endif // BOPENSAVEEDITORMODULE_P_H

