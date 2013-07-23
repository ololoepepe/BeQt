#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BCodeEditPrivate;
class BPlainTextEditExtendedPrivate;
class BLineNumberWidget;

class QHBoxLayout;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QStringList;
class QPainter;
class QBrush;
class QPaintEvent;
class QTextBlock;
class QMenu;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BPlainTextEdit>
#include <BeQtWidgets/private/bplaintextedit_p.h>

#include <QObject>
#include <QList>
#include <QVector>
#include <QRectF>
#include <QAbstractTextDocumentLayout>
#include <QTextCharFormat>
#include <QFuture>
#include <QFutureWatcher>
#include <QString>
#include <QChar>
#include <QPoint>

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
    public:
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
    friend class BCodeEdit;
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
    static inline void fillBackground(QPainter *painter, const QRectF &rect, QBrush brush,
                                      QRectF gradientRect = QRectF());
public:
    void init();
    void emulateShiftPress();
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
public Q_SLOTS:
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
public:
    typedef QFuture<ProcessTextResult> ProcessTextResultFuture;
    typedef QFutureWatcher<ProcessTextResult> ProcessTextResultFutureWatcher;
public:
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
public:
    static QStringList processLine(const QString &line, int ll, BeQt::TabWidth tw);
    static ProcessTextResult processText(const QString &text, int ll, BeQt::TabWidth tw);
    static QString removeUnsupportedSymbols(const QString &text);
    static void removeUnsupportedSymbols(QString *text);
    static QList<QChar> createUnsupportedSymbols();
    static BCodeEdit::SplittedLinesRange createSplittedLinesRange();
    static QString makeBlock(const QString &text, int *length = 0);
    static void makeBlock(QString *text, int *length = 0);
    static bool testBlock(const QString &text, int *length = 0);
    static bool testBlock(const QStringList &lines, int *length = 0);
public:
    void init();
    bool eventFilter(QObject *obj, QEvent *e);
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    BCodeEdit::SplittedLinesRange deleteSelection();
    void blockHighlighter(bool block);
    void requestRehighlightBlock(const QTextBlock &block);
    void seletAll();
    void setText(const QString &txt, int asyncIfLongerThan);
    void setBuisy(bool b);
    void insertText(const QString &txt, bool asKeyPress = false);
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
public Q_SLOTS:
    void parceTaskFinished();
    void updateCursorPosition();
    void updateHasSelection();
    void updateCopyAvailable(bool available);
    void updatePasteAvailable(bool available);
    void updateUndoAvailable(bool available);
    void updateRedoAvailable(bool available);
    void emitModificationChanged(bool modified);
    void emitSelectionChanged();
    void setTextToEmptyLine();
    void delayedSetLineLength();
public:
    bool blockMode;
    int lineLength;
    int tmpLineLength;
    BeQt::TabWidth tabWidth;
    QPoint cursorPosition;
    bool hasSelection;
    bool hasBookmarks;
    bool copyAvailable;
    bool pasteAvailable;
    bool undoAvailable;
    bool redoAvailable;
    bool buisy;
    QHBoxLayout *hlt;
      BPlainTextEditExtended *ptedt;
      BLineNumberWidget *lnwgt;
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
};

#endif // BCODEEDIT_P_H
