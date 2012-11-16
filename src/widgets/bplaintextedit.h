#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;
class QPaintEvent;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

class B_WIDGETS_EXPORT BPlainTextEdit : public QPlainTextEdit, public BBase
{
    B_DECLARE_PRIVATE(BPlainTextEdit)
    Q_OBJECT
public:
    enum SelectionMode
    {
        NormalSelection,
        BlockSelection
    };
    //
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
    //
    void setDragEnabled(bool b);
    void setSelectionMode(SelectionMode mode);
    bool dragEnabled() const;
    SelectionMode mode() const;
protected:
    BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
    //
    QMimeData *createMimeDataFromSelection() const;
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

