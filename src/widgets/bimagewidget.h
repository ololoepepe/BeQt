#ifndef BIMAGEWIDGET_H
#define BIMAGEWIDGET_H

class BImageWidgetPrivate;

class QPixmap;
class QPicture;
class QImage;
class QIcon;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QWidget>

/*============================================================================
================================ BImageWidget ================================
============================================================================*/

class B_WIDGETS_EXPORT BImageWidget : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BImageWidget)
public:
    explicit BImageWidget(QWidget *parent = 0);
    explicit BImageWidget(const QPixmap &pixmap, QWidget *parent = 0);
    explicit BImageWidget(const QPicture &picture, QWidget *parent = 0);
    explicit BImageWidget(const QImage &image, QWidget *parent = 0);
    explicit BImageWidget(const QIcon &icon, QWidget *parent = 0);
    explicit BImageWidget(const QString &fileName, QWidget *parent = 0);
    ~BImageWidget();
protected:
    explicit BImageWidget(BImageWidgetPrivate &d, QWidget *parent = 0);
public:
    bool hasScaledContents() const;
    QPicture picture() const;
    QPixmap pixmap() const;
    QImage image() const;
    QIcon icon() const;
    QString imageFileName() const;
public Q_SLOTS:
    void setPixmap(const QPixmap &p);
    void setPicture(const QPicture &p);
    void setImage(const QImage &img);
    void setIcon(const QIcon &icn);
    void setImage(const QString &fileName);
    void setScaledContents(bool b);
private:
    Q_DISABLE_COPY(BImageWidget)
};

#endif // BIMAGEWIDGET_H
