#include "application.h"
#include "settingstab.h"

#include <BPluginsSettingsTab>

Application::Application() :
    BApplication()
{
}

//

BSettingsDialog::SettingsTabMap Application::settingsTabMap() const
{
    BSettingsDialog::SettingsTabMap m;
    m.insert( "myapp", new SettingsTab(firstName, secondName) );
    m.insert("plugins", new BPluginsSettingsTab);
    return m;
}

void Application::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    const QVariantMap &m = s.value("myapp");
    firstName = m.value("first name").toString();
    secondName = m.value("second name").toString();
}
