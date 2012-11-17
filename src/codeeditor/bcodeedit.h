#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;
class BPlainTextEdit;

class QString;
class QPoint;
class QFont;
class QSyntaxHighlighter;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>
#include <QTextDocument>

class BCodeEdit : public QWidget, public BBase
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
    struct BracketPair
    {
        QString opening;
        QString closing;
        //
        BracketPair(const QString &op, const QString &cl)
        {
            opening = op;
            closing = cl;
        }
    };
    //
    static QList<BracketPair> defaultBrackets();
    //
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
    //Setters
    void setReadOnly(bool ro);
    void setEditFont(const QFont &fnt);
    void setEditMode(EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(TabWidth tw);
    void setMaximumBookmarkCount(int count);
    void setBracketHighlightingEnabled(bool enabled);
    void setRecognizedBrackets(const QList<BracketPair> &list);
    void setHighlighter(QSyntaxHighlighter *highlighter);
    //Getters
    bool isReadOnly() const;
    bool isModified() const;
    bool hasSelection() const;
    bool hasBookmarks() const;
    bool isCutAvailable() const;
    bool isCopyAvailable() const;
    bool isPasteAvailable() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    QFont editFont() const;
    EditMode editMode() const;
    int editLineLength() const;
    TabWidth editTabWidth() const;
    int maximumBookmarkCount() const;
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
    void setText(const QString &txt);
    void switchMode();
    QList<SplittedLinesRange> insertText(const QString &txt);
    void moveCursor(const QPoint &pos);
    void selectText(const QPoint &start, const QPoint &end);
    void deselectText();
    void makeBookmark();
    void removeBookmark(int index);
    void removeLastBookmark();
    bool gotoBookmark(int index);
    bool gotoNextBookmark();
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
signals:
    void editModeChanged(EditMode mode);
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

