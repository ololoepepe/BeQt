#ifndef BINPUTFIELD_H
#define BINPUTFIELD_H

class BInputFieldPrivate;

class QSize;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>

/*============================================================================
================================ BInputField =================================
============================================================================*/

class B_WIDGETS_EXPORT BInputField : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BInputField)
public:
    enum ShowStyle
    {
        ShowAlways,
        ShowIfValid,
        ShowIfInvalid,
        ShowNever
    };
public:
    explicit BInputField(QWidget *parent = 0);
    explicit BInputField(ShowStyle s, QWidget *parent = 0);
    ~BInputField();
public:
    QSize sizeHint() const;
    void setShowStyle(ShowStyle s);
    void addWidget(QWidget *w);
    void insertWidget(int index, QWidget *w);
    void removeWidget(QWidget *w);
    ShowStyle showStyle() const;
    bool isValid() const;
public Q_SLOTS:
    void setValid(bool b);
private:
    Q_DISABLE_COPY(BInputField)
};

#endif // BINPUTFIELD_H
