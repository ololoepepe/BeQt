#ifndef BSETTINGSDIALOG_H
#define BSETTINGSDIALOG_H

class BAbstractSettingsTab;
class BSettingsDialogPrivate;

class QWidget;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QDialog>
#include <QMap>
#include <QVariantMap>

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

class B_WIDGETS_EXPORT BSettingsDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSettingsDialog)
public:
    enum Navigation
    {
        ListNavigation,
        TabbedNavigation
    };
public:
    typedef QMap<QString, BAbstractSettingsTab *> SettingsTabMap;
    typedef QMap<QString, QVariantMap> SettingsMap;
public:
    explicit BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent = 0);
    explicit BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation, QWidget *parent = 0);
    ~BSettingsDialog();
protected:
    explicit BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent = 0);
public:
    bool isValid() const;
    SettingsMap settingsMap() const;
private:
    Q_DISABLE_COPY(BSettingsDialog)
};

#endif // BSETTINGSDIALOG_H
