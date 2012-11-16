#ifndef APPLICATION_H
#define APPLICATION_H

class QString;
class QByteArray;
class QSettings;

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
    QByteArray pwstate;
private slots:
    void loadSettingsSlot(QSettings *s);
    void saveSettingsSlot(QSettings *s);
};

#endif // APPLICATION_H
