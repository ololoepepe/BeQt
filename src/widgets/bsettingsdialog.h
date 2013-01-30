#ifndef BSETTINGSDIALOG_H
#define BSETTINGSDIALOG_H

class BAbstractSettingsTab;
class BSettingsDialogPrivate;

class QWidget;
class QStringList;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QDialog>
#include <QList>

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
    explicit BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, QWidget *parent = 0);
    explicit BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, Navigation navigation, QWidget *parent = 0);
    explicit BSettingsDialog(BAbstractSettingsTab *tab, QWidget *parent = 0);
    ~BSettingsDialog();
protected:
    explicit BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent = 0);
public:
    bool isValid() const;
private:
    Q_DISABLE_COPY(BSettingsDialog)
};

#endif // BSETTINGSDIALOG_H
