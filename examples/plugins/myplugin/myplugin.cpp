#include "myplugin.h"
#include "settingstab.h"

#include <BPluginWrapper>
#include <BApplication>
#include <BPersonInfoProvider>

#include <QPixmap>
#include <QSettings>
#include <QString>
#include <QVariant>

#include <QDebug>

MyPlugin::MyPlugin()
{
    //
}

MyPlugin::~MyPlugin()
{
    //
}

//

QString MyPlugin::type() const
{
    return "example";
}

QString MyPlugin::name() const
{
    return "My Plugin";
}

bool MyPlugin::prefereStaticInfo() const
{
    return true;
}

MyPlugin::PluginInfo MyPlugin::info() const
{
    return PluginInfo();
}

MyPlugin::PluginInfoStatic MyPlugin::staticInfo() const
{
    PluginInfoStatic sinf;
    sinf.organization = "Andrey Bogdanov";
    sinf.copyrightYears = "2012";
    sinf.website = "https://github.com/the-dark-angel/BeQt";
    sinf.description = "Test plugin for demonstration.";
    sinf.license = "MIT license";
    BPersonInfoProvider::PersonInfo pi;
    pi.name = "Andrey Bogdanov";
    pi.role = "Main developer";
    pi.site = "https://github.com/the-dark-angel";
    pi.mail = "ololoepepe@gmail.com";
    sinf.authors = BPersonInfoProvider::PersonInfoList() << pi;
    pi.role = "Translator";
    sinf.translators = BPersonInfoProvider::PersonInfoList() << pi;
    pi.name = "Miku";
    pi.role = "Muse";
    pi.site = "";
    pi.mail = "";
    sinf.thanksTo = BPersonInfoProvider::PersonInfoList() << pi;
    return sinf;
}

void MyPlugin::activate()
{
    QSettings *s = BPluginWrapper::createPluginSettingsInstance(this);
    if (!s)
        return;
    s->beginGroup("Global");
    firstName = s->value("first_name").toString();
    s->endGroup();
    s->deleteLater();
}

void MyPlugin::deactivate()
{
    QSettings *s = BPluginWrapper::createPluginSettingsInstance(this);
    if (!s)
        return;
    s->beginGroup("Global");
    s->setValue("first_name", firstName);
    s->endGroup();
    s->deleteLater();
}

QPixmap MyPlugin::pixmap() const
{
    return QPixmap(BApplication::location(BApplication::DataPath, BApplication::UserResources) +
                   "/my-plugin/images/myplugin.png");
}

BAbstractSettingsTab *MyPlugin::settingsTab() const
{
    return new SettingsTab(firstName);
}

void MyPlugin::handleSettings(const QVariantMap &s)
{
    firstName = s.value("first name").toString();
}

Q_EXPORT_PLUGIN2(myplugin, MyPlugin)
