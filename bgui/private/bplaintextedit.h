#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class QWidget;
class QPaintEvent;
class QMimeData;

#include <QPlainTextEdit>

class BPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit BPlainTextEdit(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);
    QMimeData *createMimeDataFromSelection() const;
};

#endif // BPLAINTEXTEDIT_H
