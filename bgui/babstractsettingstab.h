#ifndef BABSTRACTSETTINGSTAB_H
#define BABSTRACTSETTINGSTAB_H

#include <QWidget>
#include <QVariantMap>
#include <QString>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BAbstractSettingsTab : public QWidget
{
    Q_OBJECT
public:
    BAbstractSettingsTab();
    //
    virtual QVariantMap valueMap() const = 0;
    virtual QString title() const = 0;
};

#endif // BABSTRACTSETTINGSTAB_H
