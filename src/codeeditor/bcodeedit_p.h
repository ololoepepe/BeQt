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
private:
    Q_DISABLE_COPY(BPlainTextEditExtended)
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
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
public slots:
    void selectionChanged();
public:
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEditExtended::SelectionRange> selectionRanges;
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
    struct TestBracketResult
    {
        bool valid;
        BCodeEdit::BracketPair bracket;
        bool opening;
    };
    struct FindBracketResult
    {
        bool valid;
        int pos;
        BCodeEdit::BracketPair bracket;
    };
public:
    typedef QFuture<ProcessTextResult> ProcessTextFuture;
    typedef QFutureWatcher<ProcessTextResult> ProcessTextFutureWatcher;
public:
    static const QList<QChar> UnsupportedSymbols;
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
    static FindBracketResult findBracketPair(const BCodeEdit::BracketPair &br, bool opening, const QTextBlock &tb,
                                             int pos, const QList<BCodeEdit::BracketPair> &brlist);
    static TestBracketResult testBracket(const QString &txt, int pos, const QList<BCodeEdit::BracketPair> &brlist);
    static BCodeEdit::BracketPair createBracketPair( const QString &op, const QString &cl,
                                                     const QString &esc = QString() );
    static BCodeEdit::SplittedLinesRange createSplittedLinesRange();
    static TestBracketResult createTestBracketResult();
    static FindBracketResult createFindBracketResult();
    static bool testBracketPairsEquality(const BCodeEdit::BracketPair &bp1, const BCodeEdit::BracketPair &bp2);
    static bool testBracketPairListsEquality(const QList<BCodeEdit::BracketPair> &l1,
                                             const QList<BCodeEdit::BracketPair> &l2);
public:
    void init();
    bool eventFilter(QObject *obj, QEvent *e);
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    void deleteSelection();
    void seletAll();
    void setText(const QString &txt, int asyncIfLongerThan);
    void setBuisy(bool b);
    int replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    void highlightBrackets();
    bool testBracket(bool opening, const BCodeEdit::BracketPair *&bracket) const;
    void emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges);
    void handleReturn();
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
public slots:
    void futureWatcherFinished();
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
    QList<QTextEdit::ExtraSelection> highlightedBrackets;
    QVBoxLayout *vlt;
      BPlainTextEditExtended *ptedt;
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
};

#endif // BCODEEDIT_P_H

