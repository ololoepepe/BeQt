#ifndef BFLOWLAYOUT_H
#define BFLOWLAYOUT_H

class QLayoutItem;
class QWidget;

#include <QLayout>
#include <QRect>
#include <QList>
#include <QStyle>
#include <QSize>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BFlowLayout : public QLayout
{
    Q_OBJECT
public:
    BFlowLayout(QWidget *parent, int hSpacing = -1, int vSpacing = -1);
    BFlowLayout(int hSpacing = -1, int vSpacing = -1);
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
private:
    QList<QLayoutItem *> _m_items;
    int _m_hSpace;
    int _m_vSpace;
    //
    int _m_doLayout(const QRect &rect, bool testOnly) const;
    int _m_smartSpacing(QStyle::PixelMetric pm) const;
};

#endif // BFLOWLAYOUT_H
