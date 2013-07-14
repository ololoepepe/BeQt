#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;
class BPlainTextEdit;
class BAbstractCodeEditorDocumentPrivate;
class BCodeEditorDocument;
class BCodeEditorDocumentPrivate;

class QString;
class QPoint;
class QFont;
class BAbstractFileType;
class QColor;

#include "babstractcodeeditordocument.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>
#include <QTextDocument>
#include <QTextBlock>

/*============================================================================
================================ BCodeEdit ===================================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEdit : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEdit)

public:
    enum EditMode
    {
        NormalMode,
        BlockMode
    };
public:
    struct SplittedLinesRange
    {
        int firstLineNumber;
        int lastLineNumber;
    };
public:
    typedef BAbstractCodeEditorDocument::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
public:
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
protected:
    explicit BCodeEdit(BCodeEditPrivate &d, QWidget *parent = 0);
public:
    static void setBlockComment(QTextBlock block, int start = -1, int end = -1);
public:
    void setReadOnly(bool ro);
    void setModification(bool modified);
    void setEditFont(const QFont &fnt);
    void setEditMode(EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setLineNumberWidgetVisible(bool b);
    void setCurrentLineHighlightingEnabled(bool b);
    void setHighlightedLineColor(const QColor &c);
    void setExtraSelections(const ExtraSelectionList &list);
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    bool isReadOnly() const;
    bool isModified() const;
    bool hasSelection() const;
    bool isCutAvailable() const;
    bool isCopyAvailable() const;
    bool isPasteAvailable() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    QFont editFont() const;
    EditMode editMode() const;
    int editLineLength() const;
    BeQt::TabWidth editTabWidth() const;
    bool lineNumberWidgetVisible() const;
    bool currentLineHighlightingEnabled() const;
    QColor highlightedLineColor() const;
    ExtraSelectionList extraSelections() const;
    QPoint cursorPosition() const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    QPoint selectionStart() const;
    QPoint selectionEnd() const;
    bool isBuisy() const;
public Q_SLOTS:
    void setFocus();
    void activateWindow();
    void setText(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void switchMode();
    void insertText(const QString &txt);
    void moveCursor(const QPoint &pos);
    void selectText(const QPoint &start, const QPoint &end);
    void selectText(int start, int end);
    void selectLines(int firstLine, int lastLine);
    void selectAll();
    void deselectText();
    void cut();
    void copy();
    void paste();
    void deleteSelection();
    void undo();
    void redo();
    void highlightBrackets();
protected:
    BPlainTextEdit *innerEdit() const;
    QTextDocument *innerDocument() const;
Q_SIGNALS:
    void readOnlyChanged(bool ro);
    void modificationChanged(bool modified);
    void selectionChanged();
    void hasSelectionChanged(bool hasSelection);
    void cutAvailableChanged(bool available);
    void copyAvailableChanged(bool available);
    void pasteAvailableChanged(bool available);
    void undoAvailableChanged(bool available);
    void redoAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void cursorPositionChanged(const QPoint &pos);
    void buisyChanged(bool buisy);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
private:
    friend class BAbstractCodeEditorDocument;
    friend class BAbstractCodeEditorDocumentPrivate;
    friend class BCodeEditorDocument;
    friend class BCodeEditorDocumentPrivate;
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H
