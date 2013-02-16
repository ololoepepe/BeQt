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
================================ BOpenSaveEditorModulePrivate ================
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BOpenSaveEditorModule)
public:
    explicit BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q);
    ~BOpenSaveEditorModulePrivate();
public:
    void init();
    void checkActions();
    void resetFileHistory( const QStringList &list = QStringList() );
public slots:
    void retranslateUi();
    void codecTriggered(const QString &codecName);
    void fileTriggered();
    void resetFileHistoryMenuToolTip();
public:
    QPointer<QAction> actNewFile;
    QPointer<QAction> actOpenFiles;
    QPointer<QAction> actReopenFile;
    QPointer<QAction> actSaveFile;
    QPointer<QAction> actSaveFileAs;
    QPointer<QAction> actSaveAllFiles;
    QPointer<QAction> actCloseFile;
    QPointer<QAction> actCloseAllFiles;
    QPointer<QMenu> mnuFileHistory;
private:
    Q_DISABLE_COPY(BOpenSaveEditorModulePrivate)
};

#endif // BOPENSAVEEDITORMODULE_P_H
