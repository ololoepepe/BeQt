#ifndef BPLUGINSSETTINGSTAB_P_H
#define BPLUGINSSETTINGSTAB_P_H

class BPluginWrapper;

class QHBoxLayout;
class QListWidget;
class QVBoxLayout;
class QPushButton;

#include "bpluginssettingstab.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QList>
#include <QObject>

class B_WIDGETS_EXPORT BPluginsSettingsTabPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BPluginsSettingsTabPrivateObject(BPluginsSettingsTabPrivate *p);
    ~BPluginsSettingsTabPrivateObject();
    //
    BPluginsSettingsTabPrivate *const _m_p;
public slots:
    void lstwgtCurrentRowChanged(int currentRow);
    void btnSettingsClicked();
    void btnAboutClicked();
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivateObject)
};

class BPluginsSettingsTabPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BPluginsSettingsTab)
    B_DECLARE_PUBLIC(BPluginsSettingsTab)
public:
    explicit BPluginsSettingsTabPrivate(BPluginsSettingsTab *q);
    ~BPluginsSettingsTabPrivate();
    //
    void lstwgtCurrentRowChanged(int currentRow);
    void btnSettingsClicked();
    void btnAboutClicked();
    //
    BPluginsSettingsTabPrivateObject *const _m_o;
    //
    QList<BPluginWrapper *> plugins;
    QHBoxLayout *hlt;
      QListWidget *lstwgt;
      QVBoxLayout *vlt;
        QPushButton *btnSettings;
        QPushButton *btnAbout;
private:
    Q_DISABLE_COPY(BPluginsSettingsTabPrivate)
    //
    friend class BPluginsSettingsTabPrivateObject;
};

#endif // BPLUGINSSETTINGSTAB_P_H
