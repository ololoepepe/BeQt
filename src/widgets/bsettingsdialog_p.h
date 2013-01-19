#ifndef BSETTINGSDIALOG_P_H
#define BSETTINGSDIALOG_P_H

class QVBoxLayout;
class QTabWidget;
class QDialogButtonBox;
class QSplitter;
class QListWidget;
class QStackedWidget;
class QCheckBox;
class QPushButton;

#include "bsettingsdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QList>
#include <QMap>
#include <QApplication>
#include <QStringList>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

class BSettingsDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    explicit BSettingsDialogPrivate( BSettingsDialog *q, const QList<BAbstractSettingsTab *> &tabs,
                                     BSettingsDialog::Navigation navigation = BSettingsDialog::ListNavigation);
    ~BSettingsDialogPrivate();
public:
    void init();
public:
    const QList<BAbstractSettingsTab *> Tabs;
    const BSettingsDialog::Navigation Navigation;
public:
    bool valid;
    QVBoxLayout *vlt;
      QCheckBox *cboxAdvancedMode;
      QPushButton *btnRestoreDefault;
      QSplitter *hspltr;
        QListWidget *lstwgt;
        QStackedWidget *stkdwgt;
      QTabWidget *twgt;
      QDialogButtonBox *dlgbbox;
        //Ok
        //Cancel
public slots:
    void accepted();
    void cboxAdvancedModeStateChanged(int state);
    void btnRestoreDefaultClicked();
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

#endif // BSETTINGSDIALOG_P_H
