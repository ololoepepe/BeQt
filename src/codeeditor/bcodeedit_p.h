#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BCodeEditPrivate;
class BPlainTextEdit;

class QVBoxLayout;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QString;
class QChar;
class QStringList;
class QSyntaxHighlighter;
class QPoint;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include <QTextBlock>
#include <QTextEdit>

/*========== Code Edit Clipboard Notifier ==========*/

class BCodeEditClipboardNotifier : public QObject
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

/*========== Code Edit Private Object ==========*/

class BCodeEditPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BCodeEditPrivateObject(BCodeEditPrivate *p);
    ~BCodeEditPrivateObject();
    //
    bool eventFilter(QObject *obj, QEvent *e);
    //
    BCodeEditPrivate *const _m_p;
public slots:
    void futureWatcherFinished();
    void customContextMenuRequested(const QPoint &pos);
    void cursorPositionChanged();
    void clipboardDataAvailableChanged(bool available);
private:
    Q_DISABLE_COPY(BCodeEditPrivateObject)
};

/*========== Code Edit Private ==========*/

class BCodeEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEdit)
    Q_DECLARE_TR_FUNCTIONS(BCodeEdit)
public:
    struct ProcessTextResult
    {
        QString newText;
        QList<BCodeEdit::SplittedLinesRange> splittedLinesRanges;
    };
    struct Bookmark
    {
        QTextBlock block;
        int posInBlock;
        //
        Bookmark()
        {
            posInBlock = -1;
        }
        //
        bool operator==(const Bookmark &other) const
        {
            return block == other.block && posInBlock == other.posInBlock;
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
    //
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
    //
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    void setTextToEmptyLine();
    void deleteSelection();
    void seletAll();
    int replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    void highlightBrackets();
    void emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges);
    //KeyPress handlers
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
    //Called from BCodeEditPrivate
    void futureWatcherFinished(ProcessTextFutureWatcher *watcher);
    void popupMenu(const QPoint &pos);
    void updateCursorPosition();
    void updateClipboardDataAvailable(bool available);
    //
    static const QList<QChar> unsupportedSymbols;
    //
    BCodeEditPrivateObject *const _m_o;
    //
    bool blockMode;
    int lineLength;
    BCodeEdit::TabWidth tabWidth;
    QSyntaxHighlighter *highlighter;
    QPoint cursorPosition;
    QList<Bookmark> bookmarks;
    int maxBookmarks;
    Bookmark currentBookmark;
    QList<BCodeEdit::BracketPair> brackets;
    bool bracketsHighlighting;
    QList<QTextEdit::ExtraSelection> highlightedBrackets;
    QVBoxLayout *vlt;
      BPlainTextEdit *ptedt;
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
    //
    friend class BCodeEditPrivateObject;
};

#endif // BCODEEDIT_P_H

