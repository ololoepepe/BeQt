#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;
class BPlainTextEdit;

class QString;
class QPoint;
class QFont;
class BAbstractFileType;

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
    enum TabWidth
    {
        TabWidth2 = 2,
        TabWidth4 = 4,
        TabWidth8 = 8
    };
public:
    struct BracketPair
    {
        QString opening;
        QString closing;
        QString escape;
    };
    struct SplittedLinesRange
    {
        int firstLineNumber;
        int lastLineNumber;
    };
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
    void setEditTabWidth(TabWidth tw);
    void setFileType(BAbstractFileType *ft);
    void setRecognizedBrackets(const QList<BracketPair> &list);
    void setBracketHighlightingEnabled(bool enabled);
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
    TabWidth editTabWidth() const;
    BAbstractFileType *fileType() const;
    QList<BracketPair> recognizedBrackets() const;
    bool isBracketHighlightingEnabled() const;
    QPoint cursorPosition() const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    QPoint selectionStart() const;
    QPoint selectionEnd() const;
    bool isBuisy() const;
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
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
    void rehighlight();
protected:
    BPlainTextEdit *innerEdit() const;
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
    void fileTypeChanged(BAbstractFileType *ft);
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H
