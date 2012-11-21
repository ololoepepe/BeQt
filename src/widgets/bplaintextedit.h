#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

/*============================================================================
================================ Plain Text Edit
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEdit : public QPlainTextEdit, public BBase
{
    B_DECLARE_PRIVATE(BPlainTextEdit)
    Q_OBJECT
public:
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
    //
    void setDragEnabled(bool b);
    bool dragEnabled() const;
protected:
    BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
    //
    QMimeData *createMimeDataFromSelection() const;
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

