#ifndef APPLICATION_H
#define APPLICATION_H

class QString;

#include <BApplication>
#include <BSettingsDialog>

class Application : public BApplication
{
    Q_OBJECT
public:
    explicit Application();
protected:
    BSettingsDialog::SettingsTabMap settingsTabMap() const;
    void handleSettings(const BSettingsDialog::SettingsMap &s);
private:
    QString firstName;
    QString secondName;
};

#endif // APPLICATION_H
