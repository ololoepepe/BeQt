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

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BPluginsSettingsTab)
    Q_OBJECT
public:
    explicit BPluginsSettingsTabPrivateObject(BPluginsSettingsTabPrivate *p);
    ~BPluginsSettingsTabPrivateObject();
public slots:
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
    void btnSettingsClicked();
    void btnAboutClicked();
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivateObject)
};

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
    B_DECLARE_OBJECT(BPluginsSettingsTab)
    Q_DECLARE_TR_FUNCTIONS(BPluginsSettingsTab)
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
    //
    void lstwgtCurrentRowChanged(int currentRow);
    void lstwgtItemChanged(QListWidgetItem *item);
    void btnSettingsClicked();
    void btnAboutClicked();
    //
    QList<BPluginWrapper *> plugins;
    QHBoxLayout *hlt;
      QListWidget *lstwgt;
      QVBoxLayout *vlt;
        QPushButton *btnSettings;
        QPushButton *btnAbout;
protected:
    BPluginsSettingsTabPrivate(BPluginsSettingsTab &q, BPluginsSettingsTabPrivateObject &o);
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
};

#endif // BPLUGINSSETTINGSTAB_P_H
