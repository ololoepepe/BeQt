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

/*============================== Static public methods =====================*/

void BAbstractSettingsTab::setRowVisible(QWidget *field, bool visible)
{
    if (!field)
        return;
    QFormLayout *flt = static_cast<QFormLayout *>( field->parentWidget()->layout() );
    if (!flt)
        return;
    field->setVisible(visible);
    QWidget *l = flt->labelForField(field);
    if (!l)
        return;
    l->setVisible(visible);
}

void BAbstractSettingsTab::setRowVisible(QLayout *field, bool visible)
{
    if (!field)
        return;
    QFormLayout *flt = static_cast<QFormLayout *>( field->parentWidget()->layout() );
    if (!flt)
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

QIcon BAbstractSettingsTab::icon() const
{
    return BApplication::icon("configure");
}

bool BAbstractSettingsTab::hasAdvancedMode() const
{
    return false;
}

void BAbstractSettingsTab::setAdvancedMode(bool)
{
    //
}

bool BAbstractSettingsTab::restoreDefault()
{
    return true;
}

bool BAbstractSettingsTab::saveSettings()
{
    return true;
}
