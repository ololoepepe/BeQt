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
    void selectionChanged();
private:
    Q_DISABLE_COPY(BPlainTextEditPrivateObject)
};

class B_WIDGETS_EXPORT BPlainTextEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPlainTextEdit)
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
    BPlainTextEditPrivateObject *const _m_o;
    //
    bool drag;
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEdit::SelectionRange> selectionRanges;
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
    //
    friend class BPlainTextEditPrivateObject;
};

#endif // BPLAINTEXTEDIT_P_H

