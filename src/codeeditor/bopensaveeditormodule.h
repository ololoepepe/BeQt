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
public:
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    explicit BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(Action type) const;
    QList<QAction *> openActions(bool extended = false) const;
    QList<QAction *> saveActions(bool extended = false) const;
    QList<QAction *> closeActions(bool extended = false) const;
    QList<QAction *> actions(bool extended = false) const;
    QMenu *fileHistoryMenu() const;
    QList<QAction *> fileHistoryActions() const;
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

