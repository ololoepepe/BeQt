#ifndef BABSTRACTEDITORMODULE_H
#define BABSTRACTEDITORMODULE_H

class BAbstractEditorModulePrivate;
class BCodeEditor;
class BCodeEditorPrivate;
class BCodeEditorDocument;
class BAbstractFileType;

class QString;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QPoint>
#include <QList>

/*============================================================================
================================ Abstract Editor Module
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractEditorModule : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BAbstractEditorModule)
    Q_OBJECT
public:
    explicit BAbstractEditorModule(QObject *parent = 0);
    ~BAbstractEditorModule();
    //
    virtual QString id() const = 0;
    virtual bool isBuisy() const;
    BCodeEditor *editor() const;
protected:
    BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent = 0);
    //
    virtual void editorSet(BCodeEditor *edr);
    virtual void editorUnset(BCodeEditor *edr);
    //BCodeEdit events
    virtual void documentReadOnlyChanged(bool ro);
    virtual void documentModificationChanged(bool modified);
    virtual void documentSelectionChanged();
    virtual void documentHasSelectionChanged(bool hasSelection);
    virtual void documentCutAvailableChanged(bool available);
    virtual void documentCopyAvailableChanged(bool available);
    virtual void documentPasteAvailableChanged(bool available);
    virtual void documentUndoAvailableChanged(bool available);
    virtual void documentRedoAvailableChanged(bool available);
    virtual void documentEditModeChanged(BCodeEdit::EditMode mode);
    virtual void documentCursorPositionChanged(const QPoint &pos);
    virtual void documentBuisyChanged(bool buisy);
    virtual void documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    virtual void documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
    //BCodeEditorDocument events
    virtual void documentFileNameChanged(const QString &fn);
    virtual void documentCodecChanged(const QString &codecName);
    virtual void documentFileTypeChanged(BAbstractFileType *ft);
    //BCodeEditor events
    virtual void documentAboutToBeAdded(BCodeEditorDocument *doc);
    virtual void documentAdded(BCodeEditorDocument *doc);
    virtual void documentAboutToBeRemoved(BCodeEditorDocument *doc);
    virtual void currentDocumentChanged(BCodeEditorDocument *doc);
    virtual void fileTypesChanged();
    //
    BCodeEditorDocument *currentDocument() const;
    QList<BCodeEditorDocument *> documents() const;
private:
    Q_DISABLE_COPY(BAbstractEditorModule)
    //
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_H

