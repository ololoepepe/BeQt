#ifndef BEDITEDITORMODULE_H
#define BEDITEDITORMODULE_H

class BEditEditorModulePrivate;
class BCodeEditor;
class BAbstractCodeEditorDocument;

class QString;
class QAction;

#include "babstracteditormodule.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

/*============================================================================
================================ BEditEditorModule ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BEditEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BEditEditorModule)
public:
    enum Action
    {
        CutAction,
        CopyAction,
        PasteAction,
        UndoAction,
        RedoAction,
        SwitchModeAction
    };
    enum ActionGroup
    {
        ClipboardActionGroup,
        UndoRedoActionGroup
    };
public:
    explicit BEditEditorModule(QObject *parent = 0);
    ~BEditEditorModule();
protected:
    explicit BEditEditorModule(BEditEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(int type);
    QList<QAction *> actions(int group, bool extended = false);
    QList<QAction *> actions(bool extended = false);
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentCutAvailableChanged(bool available);
    void documentCopyAvailableChanged(bool available);
    void documentPasteAvailableChanged(bool available);
    void documentUndoAvailableChanged(bool available);
    void documentRedoAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BEditEditorModule)
};

#endif // BEDITEDITORMODULE_H
