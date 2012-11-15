#ifndef BPLAINTEXTEDIT_P_H
#define BPLAINTEXTEDIT_P_H

class BPlainTextEditPrivate;

class QTextCursor;
class QPainter;
class QBrush;

#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QRectF>
#include <QAbstractTextDocumentLayout>
#include <QVector>

class B_WIDGETS_EXPORT BPlainTextEditPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BPlainTextEditPrivateObject(BPlainTextEditPrivate *p);
    ~BPlainTextEditPrivateObject();
    //
    BPlainTextEditPrivate *const _m_p;
public slots:
    void cursorPositionChanged(const QTextCursor &cursor);
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void selectionChanged();
private:
    Q_DISABLE_COPY(BPlainTextEditPrivateObject)
};

class B_WIDGETS_EXPORT BPlainTextEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPlainTextEdit)
    Q_DECLARE_TR_FUNCTIONS(BPlainTextEdit)
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
    //
    static void fillBackground( QPainter *painter, const QRectF &rect, QBrush brush, QRectF gradientRect = QRectF() );
    //
    explicit BPlainTextEditPrivate(BPlainTextEdit *q);
    ~BPlainTextEditPrivate();
    //
    void cursorPositionChanged(const QTextCursor &cursor);
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void selectionChanged();
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
    //
    BPlainTextEditPrivateObject *const _m_o;
    //
    bool drag;
    bool blockMode;
    bool hasSelection;
    int lineLength;
    QVector<SelectionRange> selectionRanges;
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
    //
    friend class BPlainTextEditPrivateObject;
};

#endif // BPLAINTEXTEDIT_P_H

