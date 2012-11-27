#ifndef BPLUGINSSETTINGSTAB_H
#define BPLUGINSSETTINGSTAB_H

class BPluginsSettingsTabPrivate;

class QString;
class QIcon;

#include "babstractsettingstab.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QVariantMap>

/*============================================================================
================================ BPluginsSettingsTab =========================
============================================================================*/

class B_WIDGETS_EXPORT BPluginsSettingsTab : public BAbstractSettingsTab, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginsSettingsTab)
public:
    BPluginsSettingsTab();
    ~BPluginsSettingsTab();
protected:
    explicit BPluginsSettingsTab(BPluginsSettingsTabPrivate &d);
public:
    QString title() const;
    QIcon icon() const;
    QVariantMap valueMap() const;
private:
    Q_DISABLE_COPY(BPluginsSettingsTab)
};

#endif // BPLUGINSSETTINGSTAB_H
