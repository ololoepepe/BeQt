#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BTextEditorDocument;

class QWidget;
class QPaintEvent;
class QMimeData;
class QPainter;

#include <QPlainTextEdit>
#include <QRectF>

class BPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit BPlainTextEdit(BTextEditorDocument *editorDocument, QWidget *parent = 0);
    //
    BTextEditorDocument *editorDocument() const;
protected:
    void paintEvent(QPaintEvent *event);
    QMimeData *createMimeDataFromSelection() const;
private:
    static void _m_fillBackground( QPainter *painter, const QRectF &rect,
                                   QBrush brush, QRectF gradientRect = QRectF() );
    //
    BTextEditorDocument *const _m_CDocument;
};

class Testeggg : public QObject
{
    Q_OBJECT
public:
    explicit Testeggg(QObject *parent = 0) : QObject(parent) {}
};

#endif // BPLAINTEXTEDIT_H
