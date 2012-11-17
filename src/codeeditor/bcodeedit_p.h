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

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>

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
    //
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
    //
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
    //
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    void deleteSelection();
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
    //
    BCodeEditPrivateObject *const _m_o;
    //
    bool blockMode;
    int lineLength;
    BCodeEdit::TabWidth tabWidth;
    QVBoxLayout *vlt;
      BPlainTextEdit *ptedt;
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
    //
    friend class BCodeEditPrivateObject;
};

#endif // BCODEEDIT_P_H

