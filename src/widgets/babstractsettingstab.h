#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

class QString;

#include <BeQtCore/BeQt>

#include <QWidget>
#include <QtGlobal>
#include <QVariantMap>

class B_WIDGETS_EXPORT BAbstractSettingsTab : public QWidget
{
    Q_OBJECT
public:
    BAbstractSettingsTab();
    ~BAbstractSettingsTab();
    //
    virtual QString title() const = 0;
    virtual QVariantMap valueMap() const = 0;
private:
    Q_DISABLE_COPY(BAbstractSettingsTab)
};

#endif // BABSTRACTSETTINGSTAB_H
