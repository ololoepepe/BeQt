#include "settingstab.h"

#include <BApplication>
#include <BPasswordWidget>

#include <QFormLayout>
#include <QLineEdit>
#include <QString>
#include <QIcon>
#include <QByteArray>

SettingsTab::SettingsTab(const QString &firstName, const QString &secondName, const QByteArray &pwstate)
{
    QFormLayout *flt = new QFormLayout(this);
    ledt1 = new QLineEdit(firstName);
    ledt1->setText(firstName);
    flt->addRow("First name", ledt1);
    ledt2 = new QLineEdit(secondName);
    ledt2->setText(secondName);
    flt->addRow("Second name", ledt2);
    pwdwgt = new BPasswordWidget;
    pwdwgt->restoreState(pwstate);
    flt->addRow("Password", pwdwgt);
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
    m.insert( "pw state", pwdwgt->saveStateEncrypted() );
    //m.insert( "password", pwdwgt->encryptedPassword() );
    //m.insert( "char_count", pwdwgt->passwordCharCount() );
    //m.insert( "save", pwdwgt->savePassword() );
    return m;
}
