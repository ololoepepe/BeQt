#ifndef BCONTAINERWIDGET_H
#define BCONTAINERWIDGET_H

class QBoxLayout;

#include <QWidget>
#include <QWidgetList>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BContainerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BContainerWidget(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = 0);
    //
    void addWidget(QWidget *widget);
    void addWidgets(const QWidgetList &list);
    void addStretch(int stretch = 0);
    void removeWidget(QWidget *widget);
private:
    QBoxLayout *_m_blt;
      //widgets
};

#endif // BCONTAINERWIDGET_H
