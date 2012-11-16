#include "application.h"
#include "settingstab.h"

#include <BPluginsSettingsTab>
#include <BPasswordWidget>

#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QVariant>

Application::Application() :
    BApplication()
{
    connect( this, SIGNAL( settingsLoaded(QSettings *) ), this, SLOT( loadSettingsSlot(QSettings *) ) );
    connect( this, SIGNAL( settingsSaved(QSettings *) ), this, SLOT( saveSettingsSlot(QSettings *) ) );
}

//

BSettingsDialog::SettingsTabMap Application::settingsTabMap() const
{
    BSettingsDialog::SettingsTabMap m;
    m.insert( "myapp", new SettingsTab(firstName, secondName, pwstate) );
    m.insert("plugins", new BPluginsSettingsTab);
    return m;
}

void Application::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    const QVariantMap &m = s.value("myapp");
    firstName = m.value("first name").toString();
    secondName = m.value("second name").toString();
    pwstate = m.value("pw state").toByteArray();
}

//

void Application::loadSettingsSlot(QSettings *s)
{
    s->beginGroup("Common");
    firstName = s->value("first_name").toString();
    secondName = s->value("second_name").toString();
    pwstate = s->value("pw_state").toByteArray();
    if (!BPasswordWidget::stateToData(pwstate).save)
        pwstate.clear();
    s->endGroup();
}

void Application::saveSettingsSlot(QSettings *s)
{
    s->beginGroup("Common");
    s->setValue("first_name", firstName);
    s->setValue("second_name", secondName);
    if (BPasswordWidget::stateToData(pwstate).save)
        s->setValue("pw_state", pwstate);
    else
        s->remove("pw_state");
    s->endGroup();
}
