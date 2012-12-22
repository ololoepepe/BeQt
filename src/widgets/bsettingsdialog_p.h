#ifndef BSETTINGSDIALOG_P_H
#define BSETTINGSDIALOG_P_H

class QVBoxLayout;
class QTabWidget;
class QDialogButtonBox;
class QSplitter;
class QListWidget;
class QStackedWidget;

#include "bsettingsdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QList>
#include <QMap>
#include <QApplication>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

class BSettingsDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs,
                           BSettingsDialog::Navigation navigation);
    ~BSettingsDialogPrivate();
public:
    void init();
public:
    const BSettingsDialog::SettingsTabMap TabMap;
    const BSettingsDialog::Navigation Navigation;
    //
    QVBoxLayout *vlt;
      QSplitter *hspltr;
        QListWidget *lstwgt;
        QStackedWidget *stkdwgt;
      QTabWidget *twgt;
      QDialogButtonBox *dlgbbox;
        //Ok
        //Cancel
public slots:
    void accepted();
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

#endif // BSETTINGSDIALOG_P_H
