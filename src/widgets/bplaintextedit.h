#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

/*============================================================================
================================ BPlainTextEdit ==============================
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEdit : public QPlainTextEdit, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPlainTextEdit)
public:
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
protected:
    explicit BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
public:
    void setDragEnabled(bool b);
    bool dragEnabled() const;
protected:
    QMimeData *createMimeDataFromSelection() const;
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

