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

/*============================================================================
================================ Plain Text Edit Private Object
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BPlainTextEdit)
    Q_OBJECT
public:
    explicit BPlainTextEditPrivateObject(BPlainTextEditPrivate *p);
    ~BPlainTextEditPrivateObject();
public slots:
    void selectionChanged();
private:
    Q_DISABLE_COPY(BPlainTextEditPrivateObject)
};

/*============================================================================
================================ Plain Text Edit Private
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPlainTextEdit)
    B_DECLARE_OBJECT(BPlainTextEdit)
    Q_DECLARE_TR_FUNCTIONS(BPlainTextEdit)
public:
    static inline void fillBackground( QPainter *painter, const QRectF &rect,
                                       QBrush brush, QRectF gradientRect = QRectF() );
    //
    explicit BPlainTextEditPrivate(BPlainTextEdit *q);
    ~BPlainTextEditPrivate();
    //
    void selectionChanged();
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
    void emulateShiftPress();
    //
    bool drag;
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEdit::SelectionRange> selectionRanges;
protected:
    BPlainTextEditPrivate(BPlainTextEdit &q, BPlainTextEditPrivateObject &o);
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
};

#endif // BPLAINTEXTEDIT_P_H

