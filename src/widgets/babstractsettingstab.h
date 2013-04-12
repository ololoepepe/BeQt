#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

class QString;
class QIcon;
class QFormLayout;
class QLayout;

#include <BeQtCore/BeQtGlobal>
#include "bapplication.h"

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
    template <typename T> static T *labelForField(QWidget *field);
    template <typename T> static T *labelForField(QLayout *field);
public:
    static void setRowVisible(QWidget *field, bool visible);
    static void setRowVisible(QLayout *field, bool visible);
    static void setRowEnabled(QWidget *field, bool enabled);
    static void setRowEnabled(QLayout *field, bool enabled);
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

/*============================================================================
================================ BAbstractSettingsTab ========================
============================================================================*/

/*============================== Static public template methods=============*/

template <typename T> T *BAbstractSettingsTab::labelForField(QWidget *field)
{
    return BApplication::labelForField<T>(field);
}

template <typename T> T *BAbstractSettingsTab::labelForField(QLayout *field)
{
    return BApplication::labelForField<T>(field);
}

#endif // BABSTRACTSETTINGSTAB_H
