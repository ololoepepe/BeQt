#ifndef BSETTINGSDIALOG_H
#define BSETTINGSDIALOG_H

class BAbstractSettingsTab;
class BSettingsDialogPrivate;

class QWidget;

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase.h>

#include <QDialog>
#include <QMap>
#include <QVariantMap>

class B_WIDGETS_EXPORT BSettingsDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSettingsDialog)
public:
    typedef QMap<QString, BAbstractSettingsTab *> SettingsTabMap;
    typedef QMap<QString, QVariantMap> SettingsMap;
    //
    explicit BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent = 0);
    ~BSettingsDialog();
    //
    SettingsMap settingsMap() const;
protected:
    BSettingsDialog(BSettingsDialogPrivate &d);
private:
    Q_DISABLE_COPY(BSettingsDialog)
};

#endif // BSETTINGSDIALOG_H
