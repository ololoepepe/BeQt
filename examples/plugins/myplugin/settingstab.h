#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

class QLineEdit;

#include <BAbstractSettingsTab>

class SettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    SettingsTab(const QString &firstName);
    ~SettingsTab();
    //
    QString title() const;
    QVariantMap valueMap() const;
private:
    QLineEdit *ledt;
};

#endif // SETTINGSTAB_H
