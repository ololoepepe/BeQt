#include "bhelpwidget.h"
#include "bguicommon.h"

#include "../bcore/bcore.h"

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>
#include <QEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QRect>

const QRect _m_DefGeometry = QRect(200, 200, 640, 640);
//
//_m_GroupMainWindow
  const QString _m_GroupHelpWidget = "help_widget";
    const QString _m_KeyGeometry = "geometry";

//

BHelpWidget::BHelpWidget(const QString &settingsGroup, const QString &home, const QString &url, QWidget *parent) :
    QWidget(parent), _m_CSettingsGroup(settingsGroup)
{
    _m_vlt = new QVBoxLayout(this);
      _m_hltActions = new QHBoxLayout;
        _m_btnHome = new QPushButton(this);
        _m_hltActions->addWidget(_m_btnHome);
        _m_btnBack = new QPushButton(this);
        _m_hltActions->addWidget(_m_btnBack);
        _m_btnForward = new QPushButton(this);
        _m_hltActions->addWidget(_m_btnForward);
        _m_hltActions->addStretch();
        _m_btnClose = new QPushButton(this);
          connect( _m_btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );
        _m_hltActions->addWidget(_m_btnClose);
      _m_vlt->addLayout(_m_hltActions);
      _m_tbsr = new QTextBrowser(this);
        _m_tbsr->setOpenExternalLinks(true);
        _m_tbsr->setSource( QUrl(home) );
        if (home != url)
            _m_tbsr->setSource( QUrl(url) );
        connect( _m_btnHome, SIGNAL( clicked() ), _m_tbsr, SLOT( home() ) );
        connect( _m_btnBack, SIGNAL( clicked() ), _m_tbsr, SLOT( backward() ) );
        connect( _m_btnForward, SIGNAL( clicked() ), _m_tbsr, SLOT( forward() ) );
        connect( _m_tbsr, SIGNAL( sourceChanged(QUrl) ), this, SLOT( _m_updateCaption() ) );
      _m_vlt->addWidget(_m_tbsr);
    //
    _m_loadSettings();
    _m_retranslateUi();
}

//

void BHelpWidget::changeEvent(QEvent *event)
{
    if (!event || event->type() != QEvent::LanguageChange)
        return QWidget::changeEvent(event);
    _m_retranslateUi();
}

void BHelpWidget::closeEvent(QCloseEvent *event)
{
    _m_saveSettings();
    QWidget::closeEvent(event);
}

//

void BHelpWidget::_m_retranslateUi()
{
    _m_btnHome->setText( tr("Home", "btn text") );
    _m_btnBack->setText( tr("Back", "btn text") );
    _m_btnForward->setText( tr("Forward", "btn text") );
    _m_btnClose->setText( tr("Close", "btn text") );
    _m_updateCaption();
}

void BHelpWidget::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CSettingsGroup.isEmpty() )
        s->beginGroup(_m_CSettingsGroup);
    s->beginGroup(_m_GroupMainWindow);
      s->beginGroup(_m_GroupHelpWidget);
        setGeometry( s->value(_m_KeyGeometry, _m_DefGeometry).toRect() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CSettingsGroup.isEmpty() )
        s->endGroup();
}

void BHelpWidget::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CSettingsGroup.isEmpty() )
        s->beginGroup(_m_CSettingsGroup);
    s->beginGroup(_m_GroupMainWindow);
      s->beginGroup(_m_GroupHelpWidget);
        s->setValue( _m_KeyGeometry, geometry() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CSettingsGroup.isEmpty() )
        s->endGroup();
}

//

void BHelpWidget::_m_updateCaption()
{
    setWindowTitle( tr("Help", "windowTitle") + ": " + _m_tbsr->documentTitle() );
}
