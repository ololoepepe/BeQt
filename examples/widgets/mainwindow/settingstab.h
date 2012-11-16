#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

class BPasswordWidget;

class QLineEdit;
class QString;
class QIcon;
class QByteArray;

#include <BAbstractSettingsTab>
#include <BPasswordWidget>

class SettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    SettingsTab(const QString &firstName, const QString &secondName, const QByteArray &pwstate);
    //
    QString title() const;
    QIcon icon() const;
    QVariantMap valueMap() const;
private:
    QLineEdit *ledt1;
    QLineEdit *ledt2;
    BPasswordWidget *pwdwgt;
};

#endif // SETTINGSTAB_H
