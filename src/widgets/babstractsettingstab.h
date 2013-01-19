#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

class QString;
class QIcon;
class QFormLayout;
class QLayout;

#include <BeQtCore/BeQtGlobal>

#include <QWidget>
#include <QtGlobal>

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

class B_WIDGETS_EXPORT BAbstractSettingsTab : public QWidget
{
    Q_OBJECT
public:
    explicit BAbstractSettingsTab();
    ~BAbstractSettingsTab();
public:
    static void setRowVisible(QFormLayout *flt, QWidget *field, bool visible);
    static void setRowVisible(QFormLayout *flt, QLayout *field, bool visible);
public:
    virtual QString title() const = 0;
    virtual QIcon icon() const;
    virtual bool hasAdvancedMode() const;
    virtual void setAdvancedMode(bool enabled);
    virtual bool restoreDefault();
    virtual bool saveSettings();
private:
    Q_DISABLE_COPY(BAbstractSettingsTab)
};

#endif // BABSTRACTSETTINGSTAB_H
