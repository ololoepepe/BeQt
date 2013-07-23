#include "babstractsettingstab.h"
#include "bapplication.h"

#include <QWidget>
#include <QFormLayout>
#include <QList>
#include <QIcon>

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

/*============================== Static public methods======================*/

void BAbstractSettingsTab::setRowVisible(QWidget *field, bool visible)
{
    BApplication::setRowVisible(field, visible);
}

void BAbstractSettingsTab::setRowVisible(QLayout *field, bool visible)
{
    BApplication::setRowVisible(field, visible);
}

void BAbstractSettingsTab::setRowEnabled(QWidget *field, bool enabled)
{
    BApplication::setRowEnabled(field, enabled);
}

void BAbstractSettingsTab::setRowEnabled(QLayout *field, bool enabled)
{
    BApplication::setRowEnabled(field, enabled);
}

/*============================== Public methods ============================*/

QIcon BAbstractSettingsTab::icon() const
{
    return BApplication::icon("configure");
}

bool BAbstractSettingsTab::hasAdvancedMode() const
{
    return false;
}

bool BAbstractSettingsTab::isInAdvancedMode() const
{
    return false;
}

void BAbstractSettingsTab::setAdvancedMode(bool)
{
    //
}

bool BAbstractSettingsTab::hasDefault() const
{
    return false;
}

bool BAbstractSettingsTab::restoreDefault()
{
    return false;
}

QString BAbstractSettingsTab::restoreDefaultHint() const
{
    return "";
}

bool BAbstractSettingsTab::saveSettings()
{
    return true;
}
