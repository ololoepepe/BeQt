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
================================ Plugins Settings Tab Private
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
    Q_OBJECT
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
    //
    QList<BPluginWrapper *> plugins;
    QHBoxLayout *hlt;
      QListWidget *lstwgt;
      QVBoxLayout *vlt;
        QPushButton *btnSettings;
        QPushButton *btnAbout;
public slots:
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
    void btnSettingsClicked();
    void btnAboutClicked();
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
};

#endif // BPLUGINSSETTINGSTAB_P_H
