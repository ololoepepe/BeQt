#include "settingstab.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

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

QVariantMap SettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( "first name", ledt->text() );
    return m;
}
