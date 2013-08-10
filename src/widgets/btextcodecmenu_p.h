#ifndef BTEXTCODECMENU_P_H
#define BTEXTCODECMENU_P_H

#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

class QSignalMapper;

/*============================================================================
================================ BTextCodecMenuPrivate =======================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecMenuPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTextCodecMenu)
public:
    explicit BTextCodecMenuPrivate(BTextCodecMenu *q, BTextCodecMenu::Style s);
    ~BTextCodecMenuPrivate();
public:
    void init();
public Q_SLOTS:
    void retranslateUi();
    void codecSelected(const QString &cn);
public:
    const BTextCodecMenu::Style Style;
public:
    QString codecName;
    QSignalMapper *mapper;
private:
    Q_DISABLE_COPY(BTextCodecMenuPrivate)
};

#endif // BTEXTCODECMENU_P_H
