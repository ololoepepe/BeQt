#include "babstractsettingstab.h"

#include <QWidget>
#include <QFormLayout>
#include <QList>

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractSettingsTab::BAbstractSettingsTab() :
    QWidget(0)
{
    //
}

BAbstractSettingsTab::~BAbstractSettingsTab()
{
    //
}

/*============================== Static public methods =====================*/

void BAbstractSettingsTab::setRowVisible(QFormLayout *flt, QWidget *field, bool visible)
{
    if (!flt || !field)
        return;
    field->setVisible(visible);
    QWidget *l = flt->labelForField(field);
    if (!l)
        return;
    l->setVisible(visible);
}

void BAbstractSettingsTab::setRowVisible(QFormLayout *flt, QLayout *field, bool visible)
{
    if (!flt || !field)
        return;
    foreach ( int i, bRange(0, field->count() - 1) )
    {
        QWidget *wgt = field->itemAt(i)->widget();
        if (wgt)
            wgt->setVisible(visible);
    }
    QWidget *l = flt->labelForField(field);
    if (!l)
        return;
    l->setVisible(visible);
}

/*============================== Public methods ============================*/

bool BAbstractSettingsTab::preconfirm() const
{
    return true;
}

bool BAbstractSettingsTab::hasAdvancedMode() const
{
    return false;
}

void BAbstractSettingsTab::setAdvancedMode(bool enabled)
{
    //
}

bool BAbstractSettingsTab::restoreDefault()
{
    return true;
}
