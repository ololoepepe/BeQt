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
#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include <QTextBlock>
#include <QTextEdit>

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
        BAbstractFileType::BracketPair bracket;
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
        BAbstractFileType::BracketPair bracket;
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
    static FindBracketResult findBracketPair(const BAbstractFileType::BracketPair &br,
                                             bool opening, const QTextBlock &tb, int pos,
                                             const QList<BAbstractFileType::BracketPair> &brlist);
    static TestBracketResult testBracket(const QString &txt, int pos,
                                         const QList<BAbstractFileType::BracketPair> &brlist);
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
    bool bracketsHighlighting;
    QSyntaxHighlighter *highlighter;
    QPoint cursorPosition;
    bool hasSelection;
    bool hasBookmarks;
    bool copyAvailable;
    bool pasteAvailable;
    bool undoAvailable;
    bool redoAvailable;
    BAbstractFileType *fileType;
    QList<BAbstractFileType::BracketPair> brackets;

    QList<QTextEdit::ExtraSelection> highlightedBrackets;
    QVBoxLayout *vlt;
      BPlainTextEdit *ptedt;
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

