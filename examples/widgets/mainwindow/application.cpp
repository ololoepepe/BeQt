#include "application.h"
#include "settingstab.h"

#include <BPluginsSettingsTab>

#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QVariant>

Application::Application() :
    BApplication()
{
    cc = 0;
    save = false;
    connect( this, SIGNAL( settingsLoaded(QSettings *) ), this, SLOT( loadSettingsSlot(QSettings *) ) );
    connect( this, SIGNAL( settingsSaved(QSettings *) ), this, SLOT( saveSettingsSlot(QSettings *) ) );
}

//

BSettingsDialog::SettingsTabMap Application::settingsTabMap() const
{
    BSettingsDialog::SettingsTabMap m;
    m.insert( "myapp", new SettingsTab(firstName, secondName, password, cc, save) );
    m.insert("plugins", new BPluginsSettingsTab);
    return m;
}

void Application::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    const QVariantMap &m = s.value("myapp");
    firstName = m.value("first name").toString();
    secondName = m.value("second name").toString();
    password = m.value("password").toByteArray();
    cc = m.value("char_count").toInt();
    save = m.value("save").toBool();
}

//

void Application::loadSettingsSlot(QSettings *s)
{
    s->beginGroup("Common");
    firstName = s->value("first_name").toString();
    secondName = s->value("second_name").toString();
    save = s->value("save").toBool();
    if (save)
    {
        password = s->value("password").toByteArray();
        cc = s->value("char_count", -1).toInt();
    }
    s->endGroup();
}

void Application::saveSettingsSlot(QSettings *s)
{
    s->beginGroup("Common");
    s->setValue("first_name", firstName);
    s->setValue("second_name", secondName);
    s->setValue("save", save);
    if (save)
    {
        s->setValue("password", password);
        s->setValue("char_count", cc);
    }
    else
    {
        s->remove("password");
        s->remove("char_count");
    }
    s->endGroup();
}
