#include "application.h"
#include "settingstab.h"

Application::Application() :
    BApplication()
{
}

//

BSettingsDialog::SettingsTabMap Application::settingsTabMap() const
{
    BSettingsDialog::SettingsTabMap m;
    m.insert( "myapp", new SettingsTab(firstName, secondName) );
    return m;
}

void Application::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    const QVariantMap &m = s.value("myapp");
    firstName = m.value("first name").toString();
    secondName = m.value("second name").toString();
}
