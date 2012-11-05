#ifndef BFLOWLAYOUT_H
#define BFLOWLAYOUT_H

class BFlowLayoutPrivate;

class QLayoutItem;
class QWidget;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase.h>

#include <QLayout>
#include <QRect>
#include <QList>
#include <QStyle>
#include <QSize>

class B_WIDGETS_EXPORT BFlowLayout : public QLayout, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BFlowLayout)
public:
    explicit BFlowLayout(QWidget *parent, int hSpacing = -1, int vSpacing = -1);
    explicit BFlowLayout(int hSpacing = -1, int vSpacing = -1);
    ~BFlowLayout();
    //
    void addItem(QLayoutItem *item);
    void setGeometry(const QRect &rect);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int width) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    QSize minimumSize() const;
    QSize sizeHint() const;
protected:
    BFlowLayout(BFlowLayoutPrivate &d);
private:
    Q_DISABLE_COPY(BFlowLayout)
};

#endif // BFLOWLAYOUT_H
