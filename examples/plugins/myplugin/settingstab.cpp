#include "settingstab.h"

#include <BApplication>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QIcon>

SettingsTab::SettingsTab(const QString &firstName)
{
    QHBoxLayout *hlt = new QHBoxLayout(this);
    QLabel *lbl  = new QLabel(this);
    lbl->setText("First name:");
    hlt->addWidget(lbl);
    ledt = new QLineEdit(this);
    ledt->setText(firstName);
    hlt->addWidget(ledt);
}

SettingsTab::~SettingsTab()
{
    //
}

//

QString SettingsTab::title() const
{
    return "My Plugin";
}

QIcon SettingsTab::icon() const
{
    return QIcon(BApplication::location(BApplication::DataPath, BApplication::UserResources) +
                 "/my-plugin/images/myplugin.png");
}

QVariantMap SettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( "first name", ledt->text() );
    return m;
}
