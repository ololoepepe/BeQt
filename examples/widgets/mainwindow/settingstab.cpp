#include "settingstab.h"

#include <BApplication>

#include <QFormLayout>
#include <QLineEdit>
#include <QString>
#include <QIcon>

SettingsTab::SettingsTab(const QString &firstName, const QString &secondName)
{
    QFormLayout *flt = new QFormLayout(this);
    ledt1 = new QLineEdit(firstName);
    flt->addRow( "First name", ledt1);
    ledt2 = new QLineEdit(secondName);
    flt->addRow( "Second name", ledt2);
}

//

QString SettingsTab::title() const
{
    return "MyApp";
}

QIcon SettingsTab::icon() const
{
    return QIcon(BApplication::location(BApplication::DataPath, BApplication::UserResources) + "/images/myapp.png");
}

QVariantMap SettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( "first name", ledt1->text() );
    m.insert( "second name", ledt2->text() );
    return m;
}
