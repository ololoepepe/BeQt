#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

class BPasswordWidget;

class QLineEdit;
class QString;
class QIcon;
class QByteArray;

#include <BAbstractSettingsTab>

class SettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    SettingsTab(const QString &firstName, const QString &secondName, const QByteArray &password, int cc, bool save);
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
