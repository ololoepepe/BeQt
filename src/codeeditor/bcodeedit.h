#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;
class BPlainTextEdit;

class QString;
class QPoint;
class QFont;
class QSyntaxHighlighter;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>
#include <QTextDocument>

/*============================================================================
================================ Code Edit
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEdit : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEdit)
    Q_OBJECT
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
    //
    struct BracketPair
    {
        QString opening;
        QString closing;
        QString escape;
        //
        BracketPair()
        {
            //
        }
        BracketPair( const QString &op, const QString &cl, const QString &esc = QString() )
        {
            opening = op;
            closing = cl;
            escape = esc;
        }
        bool operator==(const BracketPair &other) const
        {
            return opening == other.opening && closing == other.closing && escape == other.escape;
        }
    };
    struct SplittedLinesRange
    {
        int firstLineNumber;
        int lastLineNumber;
        //
        SplittedLinesRange()
        {
            firstLineNumber = -1;
            lastLineNumber = -1;
        }
    };
    //
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
    //Setters
    void setReadOnly(bool ro);
    void setEditFont(const QFont &fnt);
    void setEditMode(EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(TabWidth tw);
    void setHighlighter(QSyntaxHighlighter *hl);
    void setRecognizedBrackets(const QList<BracketPair> &list);
    void setBracketHighlightingEnabled(bool enabled);
    //Getters
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
    QSyntaxHighlighter *highlighter() const;
    QList<BracketPair> recognizedBrackets() const;
    bool isBracketHighlightingEnabled() const;
    QPoint cursorPosition() const;
    QString text() const;
    QString selectedText() const;
    //Operations
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
public slots:
    QList<SplittedLinesRange> setText(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void switchMode();
    QList<SplittedLinesRange> insertText(const QString &txt);
    void moveCursor(const QPoint &pos);
    void selectText(const QPoint &start, const QPoint &end);
    void deselectText();
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
signals:
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
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
protected:
    BCodeEdit(BCodeEditPrivate &d, QWidget *parent = 0);
    //
    BPlainTextEdit *innerEdit() const;
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H

