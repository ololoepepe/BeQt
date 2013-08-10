#ifndef BTEXTCODECCOMBOBOX_P_H
#define BTEXTCODECCOMBOBOX_P_H

#include "btextcodeccombobox.h"
#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ BTextCodecComboBoxPrivate ===================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecComboBoxPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTextCodecComboBox)
public:
    explicit BTextCodecComboBoxPrivate(BTextCodecComboBox *q, BTextCodecMenu::Style s);
    ~BTextCodecComboBoxPrivate();
public:
    void init();
public Q_SLOTS:
    void retranslateUi();
    void setCodecName(const QString &cn);
    void currentIndexChanged(int index);
public:
    const BTextCodecMenu::Style Style;
public:
    QString codecName;
    BTextCodecMenu *mnu;
private:
    Q_DISABLE_COPY(BTextCodecComboBoxPrivate)
};

#endif // BTEXTCODECCOMBOBOX_P_H
