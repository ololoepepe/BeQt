#ifndef BOPENSAVEEDITORMODULE_H
#define BOPENSAVEEDITORMODULE_H

#include "babstracteditormodule.h"

class BOpenSaveEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;
class QMenu;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>

/*============================================================================
================================ BOpenSaveEditorModule =======================
============================================================================*/

class B_CODEEDITOR_EXPORT BOpenSaveEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BOpenSaveEditorModule)
public:
    enum Action
    {
        NewFileAction,
        OpenFilesAction,
        ReopenAction,
        SaveFileAction,
        SaveFileAsAction,
        SaveAllFilesAction,
        CloseFileAction,
        CloseAllFilesAction
    };
    enum ActionGroup
    {
        OpenActionGroup,
        SaveActionGroup,
        CloseActionGroup
    };
public:
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    explicit BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(int type);
    QList<QAction *> actions(int group, bool extended = false);
    QList<QAction *> actions(bool extended = false);
    QMenu *fileHistoryMenu();
    QList<QAction *> fileHistoryActions();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentReadOnlyChanged(bool ro);
    void documentModificationChanged(bool modified);
    void documentBuisyChanged(bool buisy);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void fileHistoryChanged(const QStringList &list);
private:
    Q_DISABLE_COPY(BOpenSaveEditorModule)
};

#endif // BOPENSAVEEDITORMODULE_H
