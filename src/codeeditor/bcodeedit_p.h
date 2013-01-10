#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BCodeEditPrivate;
class BPlainTextEditExtendedPrivate;
class BCodeEditParseTask;
class BLineNumberWidget;
class BCodeEdit;

class QHBoxLayout;
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
class QThreadPool;
class QResizeEvent;
class QRect;
class QSize;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BPlainTextEdit>
#include <BeQtWidgets/private/bplaintextedit_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QTextBlock>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QVector>
#include <QRectF>
#include <QAbstractTextDocumentLayout>
#include <QPair>
#include <QTextCharFormat>
#include <QRunnable>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BTextBlockUserData : public QTextBlockUserData
{
public:
    explicit BTextBlockUserData(int sf = -1, int st = -1);
    ~BTextBlockUserData();
public:
    static QString textWithoutComments(const BTextBlockUserData *ud, const QString &text);
    static QString textWithoutComments(const QTextBlock &block);
    static int blockSkipFrom(const QTextBlock &block);
public:
    int skipFrom;
    int skipTo;
};

/*============================================================================
================================ BCodeEditClipboardNotifier ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditClipboardNotifier : public QObject
{
    Q_OBJECT
public:
    explicit BCodeEditClipboardNotifier();
    ~BCodeEditClipboardNotifier();
public:
    static BCodeEditClipboardNotifier *instance();
public:
    bool clipboardDataAvailable() const;
public slots:
    void dataChanged();
signals:
    void clipboardDataAvailableChanged(bool available);
protected:
    static BCodeEditClipboardNotifier *_m_self;
private:
    bool dataAvailable;
private:
    Q_DISABLE_COPY(BCodeEditClipboardNotifier)
};

/*============================================================================
================================ BPlainTextEditExtended ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BPlainTextEditExtended : public BPlainTextEdit
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPlainTextEditExtended)
public:
    enum SelectionMode
    {
        NormalSelection,
        BlockSelection
    };
public:
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
public:
    explicit BPlainTextEditExtended(QWidget *parent = 0);
    ~BPlainTextEditExtended();
protected:
    explicit BPlainTextEditExtended(BPlainTextEditExtendedPrivate &d, QWidget *parent = 0);
public:
    void setBlockMode(bool enabled);
    void updateSelection();
    bool blockMode() const;
    QVector<SelectionRange> selectionRanges() const;
protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEditExtended)
    friend class BLineNumberWidget;
    friend class BCodeEdit;
};

/*============================================================================
================================ BLineNumberWidget ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BLineNumberWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BLineNumberWidget(BPlainTextEditExtended *ptedt);
    ~BLineNumberWidget();
public:
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *e);
public:
    BPlainTextEditExtended *const Ptedt;
private:
    Q_DISABLE_COPY(BLineNumberWidget)
};

/*============================================================================
================================ BPlainTextEditExtendedPrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BPlainTextEditExtendedPrivate : public BPlainTextEditPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPlainTextEditExtended)
public:
    explicit BPlainTextEditExtendedPrivate(BPlainTextEditExtended *q);
    ~BPlainTextEditExtendedPrivate();
public:
    static inline void fillBackground( QPainter *painter, const QRectF &rect,
                                       QBrush brush, QRectF gradientRect = QRectF() );
public:
    void init();
    void emulateShiftPress();
    void lineNumberWidgetPaintEvent(QPaintEvent *e);
    int lineNumberWidgetWidth() const;
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
public slots:
    void selectionChanged();
    void updateLineNumberWidgetWidth(int newBlockCount);
    void updateLineNumberWidget(const QRect &rect, int dy);
public:
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEditExtended::SelectionRange> selectionRanges;
    BLineNumberWidget *lnwgt;
private:
    Q_DISABLE_COPY(BPlainTextEditExtendedPrivate)
};

/*============================================================================
================================ BCodeEditPrivate ============================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEdit)
public:
    struct ProcessTextResult
    {
        QString newText;
        QList<BCodeEdit::SplittedLinesRange> splittedLinesRanges;
    };
    struct FindBracketPairResult
    {
        int start;
        int end;
        const BCodeEdit::BracketPair *startBr;
        const BCodeEdit::BracketPair *endBr;
    };
public:
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
public:
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
    static QList<QChar> createUnsupportedSymbols();
    static QTextCharFormat createBracketsFormat();
    static QTextCharFormat createBracketsErrorFormat();
    static BCodeEdit::SplittedLinesRange createSplittedLinesRange();
    static FindBracketPairResult createFindBracketPairResult();
    static QTextEdit::ExtraSelection createExtraSelection( const QPlainTextEdit *edt,
                                                           const QTextCharFormat &format = QTextCharFormat() );
    static bool testBracketPairsEquality(const BCodeEdit::BracketPair &bp1, const BCodeEdit::BracketPair &bp2);
    static bool testBracketPairListsEquality(const QList<BCodeEdit::BracketPair> &l1,
                                             const QList<BCodeEdit::BracketPair> &l2);
public:
    void init();
    bool eventFilter(QObject *obj, QEvent *e);
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    BCodeEdit::SplittedLinesRange deleteSelection();
    void seletAll();
    void setText(const QString &txt, int asyncIfLongerThan);
    void setBuisy(bool b);
    int replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    void highlightBrackets();
    FindBracketPairResult findLeftBracketPair() const;
    FindBracketPairResult findRightBracketPair() const;
    bool testBracket(const QString &text, int posInBlock, bool opening, const BCodeEdit::BracketPair *&bracket) const;
    void emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges);
    void emitLineSplitted(const BCodeEdit::SplittedLinesRange &range);
    void handleReturn();
    void handleSpace();
    void handleTab();
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
public slots:
    void parceTaskFinished();
    void popupMenu(const QPoint &pos);
    void updateCursorPosition();
    void updateHasSelection();
    void updateCopyAvailable(bool available);
    void updatePasteAvailable(bool available);
    void updateUndoAvailable(bool available);
    void updateRedoAvailable(bool available);
    void emitModificationChanged(bool modified);
    void emitSelectionChanged();
    void setTextToEmptyLine();
public:
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
    bool buisy;
    BCodeEditParseTask *parceTask;
    QList<QTextEdit::ExtraSelection> highlightedBrackets;
    QHBoxLayout *hlt;
      BPlainTextEditExtended *ptedt;
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
};

/*============================================================================
================================ BCodeEditParseTask ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditParseTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    BCodeEditParseTask(const QString &text, int lineLength, BCodeEdit::TabWidth tabWidth);
    ~BCodeEditParseTask();
public:
    static QThreadPool *pool();
public:
    void run();
    BCodeEditPrivate::ProcessTextResult result() const;
signals:
    void finished();
private:
    static QThreadPool *tp;
private:
    const QString Text;
    const int LineLength;
    const BCodeEdit::TabWidth TabWidth;
private:
    BCodeEditPrivate::ProcessTextResult res;
private:
    Q_DISABLE_COPY(BCodeEditParseTask)
};

#endif // BCODEEDIT_P_H
