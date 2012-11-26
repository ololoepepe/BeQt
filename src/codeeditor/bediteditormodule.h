#ifndef BEDITEDITORMODULE_H
#define BEDITEDITORMODULE_H

class BEditEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;

#include "babstracteditormodule.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

/*============================================================================
================================ Edit Editor Module
============================================================================*/

class B_CODEEDITOR_EXPORT BEditEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BEditEditorModule)
    Q_DISABLE_COPY(BEditEditorModule)
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
    //
    explicit BEditEditorModule(QObject *parent = 0);
    ~BEditEditorModule();
protected:
    BEditEditorModule(BEditEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(Action type) const;
    QList<QAction *> clipboardActions() const;
    QList<QAction *> undoRedoActions() const;
    QList<QAction *> actions() const;
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentCutAvailableChanged(bool available);
    void documentCopyAvailableChanged(bool available);
    void documentPasteAvailableChanged(bool available);
    void documentUndoAvailableChanged(bool available);
    void documentRedoAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void currentDocumentChanged(BCodeEditorDocument *doc);
};

#endif // BEDITEDITORMODULE_H
