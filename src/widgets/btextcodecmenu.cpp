#include "btextcodecmenu.h"
#include "btextcodecmenu_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQt>

#include <QMenu>
#include <QString>
#include <QTextCodec>
#include <QSignalMapper>
#include <QList>
#include <QAction>

/*============================================================================
================================ BTextCodecMenuPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecMenuPrivate::BTextCodecMenuPrivate(BTextCodecMenu *q, BTextCodecMenu::Style s) :
    BBasePrivate(q), Style(s)
{
    //
}

BTextCodecMenuPrivate::~BTextCodecMenuPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTextCodecMenuPrivate::init()
{
    mapper = new QSignalMapper(this);
    q_func()->setMapping(this, SLOT(codecSelected(QString)));
    if (BTextCodecMenu::StructuredStyle == Style)
    {
        foreach (BeQt::CodecsGroup gr, BeQt::codecsGroups())
        {
            QMenu *submnu = q_func()->addMenu("");
            submnu->setProperty("beqt/codec_group", gr);
            foreach (const QString &cn, BeQt::codecNamesForGroup(gr))
            {
                QAction *act = submnu->addAction("");
                act->setProperty("beqt/codec_name", cn);
                bSetMapping(mapper, act, SIGNAL(triggered()), cn);
            }
        }
    }
    else
    {
        foreach (const QString &cn, BeQt::supportedCodecsNames())
        {
            QAction *act = q_func()->addAction("");
            act->setProperty("beqt/codec_name", cn);
            bSetMapping(mapper, act, SIGNAL(triggered()), cn);
        }
    }
    retranslateUi();
}

/*============================== Public slots ==============================*/

void BTextCodecMenuPrivate::retranslateUi()
{
    QList<QAction *> actions;
    foreach (QAction *act, q_func()->actions())
    {
        QMenu *submnu = act->menu();
        if (!submnu)
        {
            actions << act;
            continue;
        }
        BeQt::CodecsGroup gr = BeQt::codecsGroupFromInt(submnu->property("beqt/codec_group").toInt());
        if (BeQt::InvalidGroup == gr)
            continue;
        submnu->setTitle(BeQt::codecsGroupName(gr));
        actions << submnu->actions();
    }
    foreach (QAction *act, actions)
    {
        QString cn = act->property("beqt/codec_name").toString();
        if (cn.isEmpty() || !BeQt::supportedCodecsNames().contains(cn))
            continue;
        act->setText(BeQt::fullCodecName(cn));
    }
}

void BTextCodecMenuPrivate::codecSelected(const QString &cn)
{
    codecName = cn;
}

/*============================================================================
================================ BTextCodecMenu ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecMenu::BTextCodecMenu(QWidget *parent) :
    QMenu(parent), BBase(*new BTextCodecMenuPrivate(this, StructuredStyle))
{
    d_func()->init();
}

BTextCodecMenu::BTextCodecMenu(Style s, QWidget *parent) :
    QMenu(parent), BBase(*new BTextCodecMenuPrivate(this, s))
{
    d_func()->init();
}

BTextCodecMenu::~BTextCodecMenu()
{
    //
}

/*============================== Protected constructors ====================*/

BTextCodecMenu::BTextCodecMenu(BTextCodecMenuPrivate &d, QWidget *parent) :
    QMenu(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BTextCodecMenu::setMapping(const QObject *receiver, const char *method)
{
    if (!receiver || !method)
        return;
    connect(d_func()->mapper, SIGNAL(mapped(QString)), receiver, method);
}

BTextCodecMenu::Style BTextCodecMenu::style() const
{
    return d_func()->Style;
}

QTextCodec *BTextCodecMenu::selectedCodec() const
{
    return BeQt::codec(d_func()->codecName);
}

QString BTextCodecMenu::selectedCodecName() const
{
    return d_func()->codecName;
}
