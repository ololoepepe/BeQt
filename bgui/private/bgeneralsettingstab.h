#ifndef BGENERALSETTINGSTAB_H
#define BGENERALSETTINGSTAB_H

class BPluginInterface;

class QFormLayout;
class QComboBox;
class QCheckBox;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QListWidget;
class QPushButton;
class QObject;

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
    static const QString IdMultipleInstances;
    static const QString IdPlugins;
    //
    BGeneralSettingsTab(const QVariantMap &settings);
    //
    QVariantMap valueMap() const;
    QString title() const;
private:
    enum _m_PluginInfo
    {
        _m_CopyrightInfo,
        _m_AboutInfo,
        _m_WwwInfo
    };
    //
    QVBoxLayout *_m_vlt;
      QGroupBox *_m_gboxGeneral;
        QFormLayout *_m_fltGeneral;
          QComboBox *_m_cmboxLanguage;
          QCheckBox *_m_cboxMultipleInstances;
      QGroupBox *_m_gboxPlugins;
        QHBoxLayout *_m_hltPlugins;
          QListWidget *_m_lstwgt;
          QVBoxLayout *_m_vltPluginActions;
            QPushButton *_m_btnSettings;
            QPushButton *_m_btnCopyright;
            QPushButton *_m_btnAbout;
            QPushButton *_m_btnWww;
            //stretch
    //
    void _m_initGboxGeneral(const QLocale &currentLocale, bool multipleInstances);
    void _m_initGboxPlugins(const QList<QObject *> &plugins);
    void _m_showCurrentPluginInfo(_m_PluginInfo type);
private slots:
    void _m_lstwgtCurrentRowChanged(int currentRow);
    void _m_btnSettingsClicked();
    void _m_btnCopyrightClicked();
    void _m_btnAboutClicked();
    void _m_btnWwwClicked();
};

#endif // BGENERALSETTINGSTAB_H
