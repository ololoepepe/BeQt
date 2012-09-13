#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BTextEditorDocument;

class QWidget;
class QPaintEvent;
class QMimeData;

#include <QPlainTextEdit>

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
    BTextEditorDocument *const _m_CDocument;
};

#endif // BPLAINTEXTEDIT_H
