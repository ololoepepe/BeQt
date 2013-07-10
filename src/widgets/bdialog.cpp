#include "bdialog.h"
#include "bdialog_p.h"

#include <BeQtCore/BeQt>

#include <QObject>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QString>
#include <QList>

#include <QDebug>

/*============================================================================
================================ BDialogPrivate ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BDialogPrivate::BDialogPrivate(BDialog *q) :
    BBasePrivate(q)
{
    //
}

BDialogPrivate::~BDialogPrivate()
{
    resetWidget();
}

/*============================== Public methods ============================*/

void BDialogPrivate::init()
{
    wgt = 0;
    vlt = new QVBoxLayout(q_func());
      dlgbbox = new QDialogButtonBox;
      vlt->addWidget(dlgbbox);
}

void BDialogPrivate::resetWidget(QWidget *w)
{
    if (wgt)
    {
        vlt->removeWidget(wgt);
        wgt->deleteLater();
        wgt = 0;
    }
    wgt = w;
    if (wgt)
    {
        wgt->setParent(q_func());
        vlt->insertWidget(0, wgt);
    }
}

/*============================================================================
================================ BDialog =====================================
============================================================================*/

/*============================== Public constructors =======================*/

BDialog::BDialog(QWidget *parent) :
    QDialog(parent), BBase(*new BDialogPrivate(this))
{
    d_func()->init();
}

BDialog::BDialog(QDialogButtonBox::StandardButtons buttons, QWidget *parent) :
    QDialog(parent), BBase(*new BDialogPrivate(this))
{
    d_func()->init();
    d_func()->dlgbbox->setStandardButtons(buttons);
}

BDialog::~BDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BDialog::BDialog(BDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BDialog::setWidget(QWidget *w)
{
    d_func()->resetWidget(w);
}

void BDialog::setStretchEnabled(bool b)
{
    if (stretchEnabled() == b)
        return;
    d_func()->vlt->insertStretch(d_func()->vlt->count() - 1);
}

void BDialog::setCenterButtons(bool center)
{
    d_func()->dlgbbox->setCenterButtons(center);
}

void BDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
    d_func()->dlgbbox->setStandardButtons(buttons);
}

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const char *method)
{
    addButton(button, role, this, method);
}

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QObject *object,
                        const char *method)
{
    addButton(button, role, QList<BeQt::Target>() << BeQt::target(object, method));
}

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets)
{
    d_func()->dlgbbox->addButton(button, role);
    if (button)
    {
        foreach (const BeQt::Target &t, targets)
        {
            if (t.first && t.second)
                connect(button, SIGNAL(clicked()), t.first, t.second);
        }
    }
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role, const char *method)
{
    return addButton(text, role, this, method);
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QObject *object,
                                const char *method)
{
    return addButton(text, role, QList<BeQt::Target>() << BeQt::target(object, method));
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role,
                                const QList<BeQt::Target> &targets)
{
    QPushButton *btn = d_func()->dlgbbox->addButton(text, role);
    if (btn)
    {
        foreach (const BeQt::Target &t, targets)
        {
            if (t.first && t.second)
                connect(btn, SIGNAL(clicked()), t.first, t.second);
        }
    }
    return btn;
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const char *method)
{
    return addButton(button, this, method);
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const QObject *object, const char *method)
{
    return addButton(button, QList<BeQt::Target>() << BeQt::target(object, method));
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const QList<BeQt::Target> &targets)
{
    QPushButton *btn = d_func()->dlgbbox->addButton(button);
    if (btn)
    {
        foreach (const BeQt::Target &t, targets)
        {
            if (t.first && t.second)
                connect(btn, SIGNAL(clicked()), t.first, t.second);
        }
    }
    return btn;
}

void BDialog::removeButton(QAbstractButton *button)
{
    d_func()->dlgbbox->removeButton(button);
    if (button)
        button->disconnect(SIGNAL(clicked()));
}

QWidget *BDialog::widget() const
{
    return d_func()->wgt;
}

bool BDialog::stretchEnabled() const
{
    return d_func()->vlt->count() == (d_func()->wgt ? 3 : 2);
}

QDialogButtonBox *BDialog::buttonBox() const
{
    return d_func()->dlgbbox;
}

bool BDialog::centerButtons() const
{
    return d_func()->dlgbbox->centerButtons();
}

QDialogButtonBox::StandardButton BDialog::standardButton(QAbstractButton *button) const
{
    return d_func()->dlgbbox->standardButton(button);
}

QDialogButtonBox::StandardButtons BDialog::standardButtons() const
{
    return d_func()->dlgbbox->standardButtons();
}

QPushButton *BDialog::button(QDialogButtonBox::StandardButton which) const
{
    return d_func()->dlgbbox->button(which);
}

QDialogButtonBox::ButtonRole BDialog::buttonRole(QAbstractButton *button) const
{
    return d_func()->dlgbbox->buttonRole(button);
}

QList<QAbstractButton *> BDialog::buttons() const
{
    return d_func()->dlgbbox->buttons();
}
