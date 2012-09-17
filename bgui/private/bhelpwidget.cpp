#include "bhelpwidget.h"

#include "../bcore/bcore.h"

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>
#include <QSettings>
#include <QRect>

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
    _m_retranslateUi();
    connect( BCore::instance(), SIGNAL( localeChanged() ), this, SLOT( _m_retranslateUi() ) );
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

void BHelpWidget::_m_updateCaption()
{
    setWindowTitle( tr("Help", "windowTitle") + ": " + _m_tbsr->documentTitle() );
}
