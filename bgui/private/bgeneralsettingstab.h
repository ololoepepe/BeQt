#ifndef BGENERALSETTINGSTAB_H
#define BGENERALSETTINGSTAB_H

class QFormLayout;
class QComboBox;

#include "../babstractsettingstab.h"

#include <QLocale>
#include <QList>
#include <QVariantMap>
#include <QString>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BGeneralSettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    static const QString Id;
    static const QString IdLocale;
    //
    BGeneralSettingsTab(const QVariantMap &settings);
    //
    QVariantMap valueMap() const;
    QString title() const;
private:
    QFormLayout *_m_flt;
      QComboBox *_m_cmboxLanguage;
};

#endif // BGENERALSETTINGSTAB_H
