#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

class QLineEdit;
class QString;

#include <BAbstractSettingsTab>

class SettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    SettingsTab(const QString &firstName, const QString &secondName);
    //
    QString title() const;
    QVariantMap valueMap() const;
private:
    QLineEdit *ledt1;
    QLineEdit *ledt2;
};

#endif // SETTINGSTAB_H
