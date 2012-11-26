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

MyPlugin::PluginInfo MyPlugin::info() const
{
    PluginInfo inf;
    inf.organization = "Andrey Bogdanov";
    inf.copyrightYears = "2012";
    inf.website = "https://github.com/the-dark-angel/BeQt";
    inf.description = "Test plugin for demonstration.";
    inf.license = "MIT license";
    BPersonInfoProvider::PersonInfo pi;
    pi.name = "Andrey Bogdanov";
    pi.role = "Main developer";
    pi.site = "https://github.com/the-dark-angel";
    pi.mail = "ololoepepe@gmail.com";
    inf.authors = BPersonInfoProvider::PersonInfoList() << pi;
    pi.role = "Translator";
    inf.translators = BPersonInfoProvider::PersonInfoList() << pi;
    pi.name = "Miku";
    pi.role = "Muse";
    pi.site = "";
    pi.mail = "";
    inf.thanksTo = BPersonInfoProvider::PersonInfoList() << pi;
    return inf;
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
