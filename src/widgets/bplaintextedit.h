#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;
class QPaintEvent;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>
#include <QVector>

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
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
    //
    void setDragEnabled(bool b);
    void setSelectionMode(SelectionMode mode);
    bool dragEnabled() const;
    SelectionMode mode() const;
    QVector<SelectionRange> selectionRanges() const;
protected:
    BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
    //
    QMimeData *createMimeDataFromSelection() const;
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

