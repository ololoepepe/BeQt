#include "btextcodeccombobox.h"
#include "btextcodeccombobox_p.h"
#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQt>

#include <QComboBox>
#include <QMenu>
#include <QString>
#include <QTextCodec>
#include <QAction>
#include <QMetaObject>
#include <QVariant>

/*============================================================================
================================ BTextCodecComboBoxPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecComboBoxPrivate::BTextCodecComboBoxPrivate(BTextCodecComboBox *q, BTextCodecMenu::Style s) :
    BBasePrivate(q), Style(s)
{
    //
}

BTextCodecComboBoxPrivate::~BTextCodecComboBoxPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTextCodecComboBoxPrivate::init()
{
    if (BTextCodecMenu::StructuredStyle == Style)
    {
        mnu = new BTextCodecMenu(BTextCodecMenu::StructuredStyle, q_func());
        mnu->setMapping(this, SLOT(setCodecName(QString)));
        QAction *act = mnu->actions().first()->menu()->actions().first();
        codecName = act->property("beqt/codec_name").toString();
        q_func()->addItem(act->text());
    }
    else
    {
        mnu = 0;
        connect(q_func(), SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        foreach (const QString &cn, BeQt::supportedCodecsNames())
            q_func()->addItem("", cn);
        retranslateUi();
    }
}

/*============================== Public slots ==============================*/

void BTextCodecComboBoxPrivate::retranslateUi()
{
    if (BTextCodecMenu::StructuredStyle == Style)
    {
        foreach (int i, bRangeD(0, q_func()->count() - 1))
            q_func()->setItemText(i, BeQt::fullCodecName(q_func()->itemData(i).toString()));
    }
}

void BTextCodecComboBoxPrivate::setCodecName(const QString &cn)
{
    if (cn == codecName)
        return;
    codecName = cn;
    if (BTextCodecMenu::StructuredStyle == Style)
        q_func()->setItemText(0, BeQt::fullCodecName(codecName));
    QMetaObject::invokeMethod(q_func(), "codecChanged", Q_ARG(QTextCodec *, BeQt::codec(codecName)));
    QMetaObject::invokeMethod(q_func(), "codecNameChanged", Q_ARG(QString, codecName));
}

void BTextCodecComboBoxPrivate::currentIndexChanged(int index)
{
    setCodecName(q_func()->itemText(index));
}

/*============================================================================
================================ BTextCodecComboBox ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecComboBox::BTextCodecComboBox(QWidget *parent) :
    QComboBox(parent), BBase(*new BTextCodecComboBoxPrivate(this, BTextCodecMenu::StructuredStyle))
{
    d_func()->init();
}

BTextCodecComboBox::BTextCodecComboBox(BTextCodecMenu::Style s, QWidget *parent) :
    QComboBox(parent), BBase(*new BTextCodecComboBoxPrivate(this, s))
{
    d_func()->init();
}


BTextCodecComboBox::~BTextCodecComboBox()
{
    //
}

/*============================== Protected constructors ====================*/

BTextCodecComboBox::BTextCodecComboBox(BTextCodecComboBoxPrivate &d, QWidget *parent) :
    QComboBox(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

BTextCodecMenu::Style BTextCodecComboBox::style() const
{
    return d_func()->Style;
}

QTextCodec *BTextCodecComboBox::selectedCodec() const
{
    return BeQt::codec(d_func()->codecName);
}

QString BTextCodecComboBox::selectedCodecName() const
{
    return d_func()->codecName;
}

/*============================== Public slots ==============================*/

void BTextCodecComboBox::selectCodec(QTextCodec *codec)
{
    selectCodec(BeQt::codecName(codec));
}

void BTextCodecComboBox::selectCodec(const QString &codecName)
{
    if (codecName.isEmpty() || !BeQt::supportedCodecsNames().contains(codecName, Qt::CaseInsensitive))
        return;
    if (BTextCodecMenu::StructuredStyle == d_func()->Style)
    {
        if (codecName.isEmpty() || !BeQt::supportedCodecsNames().contains(codecName, Qt::CaseInsensitive))
            return;
        d_func()->setCodecName(codecName);
    }
    else
    {
        setCurrentIndex(findData(codecName, Qt::UserRole, 0));
    }
}

/*============================== Protected methods =========================*/

void BTextCodecComboBox::showPopup()
{
    if (d_func()->Style != BTextCodecMenu::StructuredStyle)
        return QComboBox::hidePopup();
    d_func()->mnu->setMinimumWidth(width());
    QWidget *p = parentWidget();
    QAction *act = d_func()->mnu->exec(p ? p->mapToGlobal(pos()) : pos());
    QComboBox::hidePopup();
    if (act)
    {
        d_func()->setCodecName(act->property("beqt/codec_name").toString());
        setItemText(0, act->text());
    }
}

void BTextCodecComboBox::hidePopup()
{
    if (d_func()->Style == BTextCodecMenu::StructuredStyle)
        d_func()->mnu->close();
    QComboBox::hidePopup();
}
