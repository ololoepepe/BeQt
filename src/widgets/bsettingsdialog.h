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
================================ Settings Dialog
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
    //
    typedef QMap<QString, BAbstractSettingsTab *> SettingsTabMap;
    typedef QMap<QString, QVariantMap> SettingsMap;
    //
    explicit BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent = 0);
    explicit BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation, QWidget *parent = 0);
    ~BSettingsDialog();
    //
    bool isValid() const;
    SettingsMap settingsMap() const;
protected:
    BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BSettingsDialog)
};

#endif // BSETTINGSDIALOG_H
