#include "babstractsettingstab.h"

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
