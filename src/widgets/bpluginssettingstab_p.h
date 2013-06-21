#ifndef BPLUGINSSETTINGSTAB_P_H
#define BPLUGINSSETTINGSTAB_P_H

class BPluginWrapper;

class QHBoxLayout;
class QListWidget;
class QVBoxLayout;
class QPushButton;
class QListWidgetItem;

#include "bpluginssettingstab.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QList>
#include <QObject>

/*============================================================================
================================ BPluginsSettingsTabPrivate ==================
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
public:
    void init();
public Q_SLOTS:
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
    void btnSettingsClicked();
    void btnAboutClicked();
public:
    QList<BPluginWrapper *> plugins;
    QHBoxLayout *hlt;
      QListWidget *lstwgt;
      QVBoxLayout *vlt;
        QPushButton *btnSettings;
        QPushButton *btnAbout;
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
};

#endif // BPLUGINSSETTINGSTAB_P_H
