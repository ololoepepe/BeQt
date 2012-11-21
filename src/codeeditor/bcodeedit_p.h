#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BCodeEditPrivate;
class BPlainTextEditExtendedPrivate;

class QVBoxLayout;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QString;
class QChar;
class QStringList;
class QSyntaxHighlighter;
class QPoint;
class QPainter;
class QBrush;
class QTextCursor;
class QPaintEvent;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BPlainTextEdit>
#include <BeQtWidgets/private/bplaintextedit_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include <QTextBlock>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QVector>
#include <QRectF>
#include <QAbstractTextDocumentLayout>

/*============================================================================
================================ Code Edit Clipboard Notifier
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditClipboardNotifier : public QObject
{
    Q_OBJECT
public:
    static BCodeEditClipboardNotifier *instance();
    //
    explicit BCodeEditClipboardNotifier();
    ~BCodeEditClipboardNotifier();
    //
    bool clipboardDataAvailable() const;
    //
    static BCodeEditClipboardNotifier *_m_self;
signals:
    void clipboardDataAvailableChanged(bool available);
private:
    Q_DISABLE_COPY(BCodeEditClipboardNotifier)
    //
    bool dataAvailable;
public slots:
    void dataChanged();
};

/*============================================================================
================================ Plain Text Edit Extended
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditExtended : public BPlainTextEdit
{
    B_DECLARE_PRIVATE(BPlainTextEditExtended)
    Q_OBJECT
public:
    enum SelectionMode
    {
        NormalSelection,
        BlockSelection
    };
    //
    struct SelectionRange
    {
        int start;
        int end;
        //
        SelectionRange()
        {
            start = -1;
            end = -1;
        }
    };
    //
    explicit BPlainTextEditExtended(QWidget *parent = 0);
    ~BPlainTextEditExtended();
    //
    void setBlockMode(bool enabled);
    bool blockMode() const;
    QVector<SelectionRange> selectionRanges() const;
protected:
    BPlainTextEditExtended(BPlainTextEditExtendedPrivate &d, QWidget *parent = 0);
    //
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEditExtended)
};

/*============================================================================
================================ Plain Text Edit Extended Private
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditExtendedPrivate : public BPlainTextEditPrivate
{
    B_DECLARE_PUBLIC(BPlainTextEditExtended)
    Q_OBJECT
public:
    static inline void fillBackground( QPainter *painter, const QRectF &rect,
                                       QBrush brush, QRectF gradientRect = QRectF() );
    //
    explicit BPlainTextEditExtendedPrivate(BPlainTextEditExtended *q);
    ~BPlainTextEditExtendedPrivate();
    //
    void init();
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
    void emulateShiftPress();
    //
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEditExtended::SelectionRange> selectionRanges;
public slots:
    void selectionChanged();
private:
    Q_DISABLE_COPY(BPlainTextEditExtendedPrivate)
};

/*============================================================================
================================ Code Edit Private
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEdit)
    B_DECLARE_TR_FUNCTIONS(BCodeEdit, q)
    Q_OBJECT
public:
    struct ProcessTextResult
    {
        QString newText;
        QList<BCodeEdit::SplittedLinesRange> splittedLinesRanges;
    };
    struct TestBracketResult
    {
        bool valid;
        BCodeEdit::BracketPair bracket;
        bool opening;
        //
        TestBracketResult()
        {
            valid = false;
            opening = true;
        }
    };
    struct FindBracketResult
    {
        bool valid;
        int pos;
        BCodeEdit::BracketPair bracket;
        //
        FindBracketResult()
        {
            valid = false;
            pos = -1;
        }
    };
    //
    typedef QFuture<ProcessTextResult> ProcessTextFuture;
    typedef QFutureWatcher<ProcessTextResult> ProcessTextFutureWatcher;
    //
    static QStringList processLine(const QString &line, int ll, BCodeEdit::TabWidth tw);
    static ProcessTextResult processText(const QString &text, int ll, BCodeEdit::TabWidth tw);
    static QString removeUnsupportedSymbols(const QString &text);
    static void removeUnsupportedSymbols(QString &text);
    static QString removeTrailingSpaces(const QString &s);
    static void removeTrailingSpaces(QString &s);
    static QString appendTrailingSpaces(const QString &s, int ll);
    static void appendTrailingSpaces(QString &s, int ll);
    static QString replaceTabs(const QString &s, BCodeEdit::TabWidth tw);
    static void replaceTabs(QString &s, BCodeEdit::TabWidth tw);
    static void removeExtraSelections(QList<QTextEdit::ExtraSelection> &from,
                                      const QList<QTextEdit::ExtraSelection> &what);
    static FindBracketResult findBracketPair(const BCodeEdit::BracketPair &br, bool opening, const QTextBlock &tb,
                                             int pos, const QList<BCodeEdit::BracketPair> &brlist);
    static TestBracketResult testBracket(const QString &txt, int pos, const QList<BCodeEdit::BracketPair> &brlist);
    //
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
    //
    void init();
    bool eventFilter(QObject *obj, QEvent *e);
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    void deleteSelection();
    void seletAll();
    int replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    void highlightBrackets();
    void emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges);
    //KeyPress handlers
    void handleSpace();
    void handleBackspace();
    void handleCtrlBackspace();
    void handleDelete();
    void handleCtrlDelete();
    void handleEnd(bool ctrl = false);
    void handleShiftEnd();
    void handleLeft(bool shift = false);
    void handleCtrlLeft();
    void handleCtrlRight();
    void move(int key);
    //
    static const QList<QChar> unsupportedSymbols;
    //
    bool blockMode;
    int lineLength;
    BCodeEdit::TabWidth tabWidth;
    QSyntaxHighlighter *highlighter;
    QList<BCodeEdit::BracketPair> recognizedBrackets;
    bool bracketsHighlighting;
    QPoint cursorPosition;
    bool hasSelection;
    bool hasBookmarks;
    bool copyAvailable;
    bool pasteAvailable;
    bool undoAvailable;
    bool redoAvailable;
    QList<QTextEdit::ExtraSelection> highlightedBrackets;
    QVBoxLayout *vlt;
      BPlainTextEditExtended *ptedt;
public slots:
    void futureWatcherFinished();
    void popupMenu(const QPoint &pos);
    void updateCursorPosition();
    void updateHasSelection();
    void updateCopyAvailable(bool available);
    void updatePasteAvailable(bool available);
    void updateUndoAvailable(bool available);
    void updateRedoAvailable(bool available);
    //
    void emitModificationChanged(bool modified);
    void emitSelectionChanged();
    void setTextToEmptyLine();
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
};

#endif // BCODEEDIT_P_H

