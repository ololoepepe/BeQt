#ifndef BPLUGINSSETTINGSTAB_H
#define BPLUGINSSETTINGSTAB_H

class BPluginsSettingsTabPrivate;

class QString;

#include "babstractsettingstab.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QVariantMap>

class B_WIDGETS_EXPORT BPluginsSettingsTab : public BAbstractSettingsTab, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginsSettingsTab)
public:
    BPluginsSettingsTab();
    ~BPluginsSettingsTab();
    //
    QString title() const;
    QVariantMap valueMap() const;
protected:
    BPluginsSettingsTab(BPluginsSettingsTabPrivate &d);
private:
    Q_DISABLE_COPY(BPluginsSettingsTab)
};

#endif // BPLUGINSSETTINGSTAB_H
