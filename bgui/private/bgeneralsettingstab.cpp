#include "bgeneralsettingstab.h"
#include "bguiplugininterface.h"
#include "bsettingsdialog.h"

#include "../bcore/bcore.h"
#include "../bcore/bplugininterface.h"

#include <QVariantMap>
#include <QString>
#include <QLocale>
#include <QFormLayout>
#include <QComboBox>
#include <QList>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QIcon>
#include <QObject>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

Q_DECLARE_METATYPE(QObject *)

//

QString localeToString(const QLocale &locale)
{
    QString language = QLocale::languageToString( locale.language() );
    QString country = (locale.country() != QLocale::AnyCountry) ? QLocale::countryToString( locale.country() ) : "";
    QString name = locale.name();
    return language + (!country.isEmpty() ? " (" + country + ")" : "") + " [" + name + "]";
}

//

const QString BGeneralSettingsTab::Id = "beqt/general";
const QString BGeneralSettingsTab::IdLocale = "locale";
const QString BGeneralSettingsTab::IdMultipleInstances = "multiple_instances";
const QString BGeneralSettingsTab::IdPlugins = "plugins";

//

BGeneralSettingsTab::BGeneralSettingsTab(const QVariantMap &settings)
{
    _m_vlt = new QVBoxLayout(this);
      bool blocale = settings.contains(IdLocale);
      bool bminst = settings.contains(IdMultipleInstances);
      if (blocale || bminst)
      {
          _m_fltGeneral = new QFormLayout;
            if (blocale)
            {
                _m_cmboxLanguage = new QComboBox(this);
                  _m_cmboxLanguage->setEditable(false);
                  QList<QLocale> availableLocales = BCore::availableLocales();
                  for (int i = 0; i < availableLocales.size(); ++i)
                  {
                      const QLocale &l = availableLocales.at(i);
                      _m_cmboxLanguage->addItem(localeToString(l), l);
                  }
                  _m_cmboxLanguage->setCurrentIndex( _m_cmboxLanguage->findData( settings.value(IdLocale).toLocale() ) );
                _m_fltGeneral->addRow("Language:", _m_cmboxLanguage);
            }
            else
            {
                _m_cmboxLanguage = 0;
            }
            if (bminst)
            {
                _m_cboxMultipleInstances = new QCheckBox(this);
                  _m_cboxMultipleInstances->setChecked( settings.value(IdMultipleInstances, false).toBool() );
                _m_fltGeneral->addRow(tr("Enable multiple instances:", "lbl text"), _m_cboxMultipleInstances);
            }
            else
            {
                _m_cboxMultipleInstances = 0;
            }
          _m_vlt->addLayout(_m_fltGeneral);
      }
      else
      {
          _m_fltGeneral = 0;
      }
      _m_initGboxPlugins( settings.value(IdPlugins).value< QList<QObject *> >() );
}

//

QVariantMap BGeneralSettingsTab::valueMap() const
{
    QVariantMap m;
    if (_m_cmboxLanguage)
        m.insert( IdLocale, _m_cmboxLanguage->itemData( _m_cmboxLanguage->currentIndex() ) );
    if (_m_cboxMultipleInstances)
        m.insert( IdMultipleInstances, _m_cboxMultipleInstances->isChecked() );
    return m;
}

QString BGeneralSettingsTab::title() const
{
    return tr("General", "title");
}

//

void BGeneralSettingsTab::_m_initGboxPlugins(const QList<QObject *> &plugins)
{
    if ( plugins.isEmpty() )
    {
        _m_gboxPlugins = 0;
          _m_hltPlugins = 0;
            _m_lstwgt = 0;
            _m_vltPluginActions = 0;
              _m_btnSettings = 0;
              _m_btnCopyright = 0;
              _m_btnAbout = 0;
              _m_btnWww = 0;
        //
        return;
    }
    _m_gboxPlugins = new QGroupBox(tr("Extensions", "gbox title"), this);
      _m_hltPlugins = new QHBoxLayout;
        _m_lstwgt = new QListWidget;
          for (int i = 0; i < plugins.size(); ++i)
          {
              BPluginInterface *interface = qobject_cast<BPluginInterface *>( plugins.at(i) );
              QListWidgetItem *lwi = new QListWidgetItem;
              lwi->setText(interface->title() +
                           " (" + tr("version", "lswtgti text") + " " + interface->version() + ")");
              lwi->setData( Qt::UserRole, QVariant::fromValue<QObject *>( plugins.at(i) ) );
              _m_lstwgt->addItem(lwi);
          }
          connect( _m_lstwgt, SIGNAL( currentRowChanged(int) ), this, SLOT( _m_lstwgtCurrentRowChanged(int) ) );
        _m_hltPlugins->addWidget(_m_lstwgt);
        _m_vltPluginActions = new QVBoxLayout;
          _m_btnSettings = new QPushButton;
            _m_btnSettings->setEnabled(false);
            _m_btnSettings->setIcon( QIcon( BCore::beqtIcon("configure") ) );
            _m_btnSettings->setText( tr("Configure", "btn text") );
            connect( _m_btnSettings, SIGNAL( clicked() ), this, SLOT( _m_btnSettingsClicked() ) );
          _m_vltPluginActions->addWidget(_m_btnSettings);
          _m_btnCopyright = new QPushButton;
            _m_btnCopyright->setEnabled(false);
            _m_btnCopyright->setIcon( QIcon( BCore::beqtIcon("copyright") ) );
            _m_btnCopyright->setText( tr("Copyright", "btn text") );
            connect( _m_btnCopyright, SIGNAL( clicked() ), this, SLOT( _m_btnCopyrightClicked() ) );
          _m_vltPluginActions->addWidget(_m_btnCopyright);
          _m_btnAbout = new QPushButton;
            _m_btnAbout->setEnabled(false);
            _m_btnAbout->setIcon( QIcon( BCore::beqtIcon("help_about") ) );
            _m_btnAbout->setText( tr("About", "btn text") );
            connect( _m_btnAbout, SIGNAL( clicked() ), this, SLOT( _m_btnAboutClicked() ) );
          _m_vltPluginActions->addWidget(_m_btnAbout);
          _m_btnWww = new QPushButton;
            _m_btnWww->setEnabled(false);
            _m_btnWww->setIcon( QIcon( BCore::beqtIcon("gohome") ) );
            _m_btnWww->setText( tr("Open homepage", "btn text") );
            connect( _m_btnWww, SIGNAL( clicked() ), this, SLOT( _m_btnWwwClicked() ) );
          _m_vltPluginActions->addWidget(_m_btnWww);
          _m_vltPluginActions->addStretch();
        _m_hltPlugins->addLayout(_m_vltPluginActions);
      _m_gboxPlugins->setLayout(_m_hltPlugins);
    _m_vlt->addWidget(_m_gboxPlugins);
}

void BGeneralSettingsTab::_m_showCurrentPluginInfo(_m_PluginInfo type)
{
    QListWidgetItem *lwi = _m_lstwgt->currentItem();
    BPluginInterface *interface =
            lwi ? qobject_cast<BPluginInterface *>( lwi->data(Qt::UserRole).value<QObject *>() ) : 0;
    if (!interface)
        return;
    QString text;
    QString wt;
    switch (type)
    {
    case _m_CopyrightInfo:
        text = interface->copyright();
        wt = tr("Copyright", "msgbox windowTitle");
        break;
    case _m_AboutInfo:
        text = interface->about();
        wt = tr("About", "msgbox windowTitle");
        break;
    case _m_WwwInfo:
        text = interface->www();
        break;
    default:
        break;
    }
    if ( text.isEmpty() )
        return;
    if (_m_WwwInfo == type)
    {
        QDesktopServices::openUrl( QUrl::fromUserInput(text) );
    }
    else
    {
        QMessageBox msgbox(this);
        msgbox.setIcon(QMessageBox::Information);
        msgbox.setWindowTitle(wt);
        msgbox.setText(text);
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.setDefaultButton(QMessageBox::Ok);
        msgbox.exec();
    }
}

//

void BGeneralSettingsTab::_m_lstwgtCurrentRowChanged(int currentRow)
{
    bool b = currentRow >= 0;
    _m_btnSettings->setEnabled(b);
    _m_btnCopyright->setEnabled(b);
    _m_btnAbout->setEnabled(b);
    _m_btnWww->setEnabled(b);
}

void BGeneralSettingsTab::_m_btnSettingsClicked()
{
    QListWidgetItem *lwi = _m_lstwgt->currentItem();
    BGuiPluginInterface *interface =
            lwi ? qobject_cast<BGuiPluginInterface *>( lwi->data(Qt::UserRole).value<QObject *>() ) : 0;
    BAbstractSettingsTab *tab = interface ? interface->settingsTab() : 0;
    if (!tab)
        return;
    BSettingsDialog sd(tab, this);
    if (sd.exec() != BSettingsDialog::Accepted)
        return;
    interface->applySettings( sd.valueMap() );
}

void BGeneralSettingsTab::_m_btnCopyrightClicked()
{
    _m_showCurrentPluginInfo(_m_CopyrightInfo);
}

void BGeneralSettingsTab::_m_btnAboutClicked()
{
    _m_showCurrentPluginInfo(_m_AboutInfo);
}

void BGeneralSettingsTab::_m_btnWwwClicked()
{
    _m_showCurrentPluginInfo(_m_WwwInfo);
}
