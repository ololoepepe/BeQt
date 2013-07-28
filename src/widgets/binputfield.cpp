#include "binputfield.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>

#include <QDebug>

/*============================================================================
================================ BInputFieldPrivate ==========================
============================================================================*/

class BInputFieldPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BInputField)
public:
    explicit BInputFieldPrivate(BInputField *q);
    ~BInputFieldPrivate();
public:
    void init();
    void setValid(bool b);
public:
    BInputField::ShowStyle style;
    bool valid;
    QHBoxLayout *hlt;
    QLabel *lbl;
private:
    Q_DISABLE_COPY(BInputFieldPrivate)
};


/*============================================================================
================================ BInputFieldPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BInputFieldPrivate::BInputFieldPrivate(BInputField *q) :
    BBasePrivate(q)
{
    //
}

BInputFieldPrivate::~BInputFieldPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BInputFieldPrivate::init()
{
    style = BInputField::ShowAlways;
    valid = true;
    QHBoxLayout *hltw = new QHBoxLayout(q_func());
      hltw->setContentsMargins(0, 0, 0, 0);
      hlt = new QHBoxLayout;
        hlt->setContentsMargins(0, 0, 0, 0);
      hltw->addLayout(hlt);
      lbl = new QLabel;
        lbl->setScaledContents(true);
      hltw->addWidget(lbl);
    //
    setValid(false);
}

void BInputFieldPrivate::setValid(bool b)
{
    if (b == valid)
        return;
    valid = b;
    lbl->setPixmap(QPixmap(BApplication::icon(b ? "ok" : "stop").pixmap(128)));
    switch (style)
    {
    case BInputField::ShowIfValid:
        lbl->setVisible(b);
        break;
    case BInputField::ShowIfInvalid:
        lbl->setVisible(!b);
        break;
    case BInputField::ShowNever:
        lbl->setVisible(false);
        break;
    case BInputField::ShowAlways:
    default:
        lbl->setVisible(true);
        break;
    }
}

/*============================================================================
================================ BInputField =================================
============================================================================*/

/*============================== Public constructors =======================*/

BInputField::BInputField(QWidget *parent) :
    QWidget(parent), BBase(*new BInputFieldPrivate(this))
{
    d_func()->init();
}

BInputField::BInputField(ShowStyle s, QWidget *parent) :
    QWidget(parent), BBase(*new BInputFieldPrivate(this))
{
    d_func()->init();
    setShowStyle(s);
}

BInputField::~BInputField()
{
    //
}

/*============================== Public methods ============================*/

QSize BInputField::sizeHint() const
{
    QSize sh = d_func()->hlt->sizeHint();
    d_func()->lbl->setFixedSize(sh.height(), sh.height());
    return d_func()->hlt->sizeHint();
}

void BInputField::setShowStyle(ShowStyle s)
{
    if (s == d_func()->style)
        return;
    d_func()->style = s;
    bool b = d_func()->valid;
    d_func()->valid = !b;
    d_func()->setValid(b);
}

void BInputField::addWidget(QWidget *w)
{
    d_func()->hlt->addWidget(w);
}

void BInputField::insertWidget(int index, QWidget *w)
{
    d_func()->hlt->insertWidget(index, w);
}

void BInputField::removeWidget(QWidget *w)
{
    d_func()->hlt->removeWidget(w);
}

BInputField::ShowStyle BInputField::showStyle() const
{
    return d_func()->style;
}

bool BInputField::isValid() const
{
    return d_func()->valid;
}

/*============================== Public slots ==============================*/

void BInputField::setValid(bool b)
{
    d_func()->setValid(b);
}