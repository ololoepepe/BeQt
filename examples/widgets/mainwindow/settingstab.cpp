#include "settingstab.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QString>

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

QVariantMap SettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( "first name", ledt1->text() );
    m.insert( "second name", ledt2->text() );
    return m;
}
