#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

class QString;
class QIcon;

#include <BeQtCore/BeQtGlobal>

#include <QWidget>
#include <QtGlobal>
#include <QVariantMap>

/*============================================================================
================================ Abstract Settings Tab
============================================================================*/

class B_WIDGETS_EXPORT BAbstractSettingsTab : public QWidget
{
    Q_OBJECT
public:
    BAbstractSettingsTab();
    ~BAbstractSettingsTab();
    //
    virtual QString title() const = 0;
    virtual QIcon icon() const = 0;
    virtual QVariantMap valueMap() const = 0;
private:
    Q_DISABLE_COPY(BAbstractSettingsTab)
};

#endif // BABSTRACTSETTINGSTAB_H
