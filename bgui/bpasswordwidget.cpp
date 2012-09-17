#include "bpasswordwidget.h"

#include "../bcore/bcore.h"
#include "../bcore/bcryptor.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QIcon>
#include <QString>
#include <QByteArray>

BPasswordWidget::BPasswordWidget(QWidget *parent) :
    QWidget(parent)
{
    _m_hlt = new QHBoxLayout(this);
      _m_hlt->setContentsMargins(0, 0, 0, 0);
      _m_ledt = new QLineEdit(this);
        _m_ledt->setEchoMode(QLineEdit::Password);
      _m_hlt->addWidget(_m_ledt);
      _m_cboxShow = new QCheckBox(this);
        _m_cboxShow->setIcon( QIcon( BCore::beqtIcon("encrypted") ) );
        _m_cboxShow->setIconSize( QSize(24, 24) );
        connect( _m_cboxShow, SIGNAL( toggled(bool) ), this, SLOT( _m_cboxShowToggled(bool) ) );
      _m_hlt->addWidget(_m_cboxShow);
      _m_cboxSave = new QCheckBox(this);
        _m_cboxSave->setIcon( QIcon( BCore::beqtIcon("filesave") ) );
        _m_cboxSave->setIconSize( QSize(24, 24) );
      _m_hlt->addWidget(_m_cboxSave);
    _m_retranslateUi();
    connect( BCore::instance(), SIGNAL( localeChanged() ), this, SLOT( _m_retranslateUi() ) );
}

//

void BPasswordWidget::setPassword(const QString &text)
{
    _m_ledt->setText(text);
}

void BPasswordWidget::setEncryptedPassword(const QByteArray &password, const QByteArray &key)
{
    _m_ledt->setText( QString::fromAscii( BCryptor::decryptDataStatic(password, key).data() ) );
}

void BPasswordWidget::setShowPassword(bool show)
{
    _m_cboxShow->setChecked(show);
}

void BPasswordWidget::setSavePassword(bool save)
{
    _m_cboxSave->setChecked(save);
}

QString BPasswordWidget::password() const
{
    return _m_ledt->text();
}

QByteArray BPasswordWidget::encryptedPassword(const QByteArray &key) const
{
    return BCryptor::encryptDataStatic(_m_ledt->text().toAscii(), key);
}

bool BPasswordWidget::showPassword() const
{
    return _m_cboxShow->isChecked();
}

bool BPasswordWidget::savePassword() const
{
    return _m_cboxSave->isChecked();
}

//

void BPasswordWidget::_m_retranslateUi()
{
    _m_cboxShow->setToolTip( tr("Show password", "cbox toolTip") );
    _m_cboxSave->setToolTip( tr("Save password", "cbox toolTip") );
}

void BPasswordWidget::_m_cboxShowToggled(bool b)
{
    _m_cboxShow->setIcon( QIcon( BCore::beqtIcon( b ? QString("decrypted") : QString("encrypted") ) ) );
    _m_ledt->setEchoMode(b ? QLineEdit::Normal : QLineEdit::Password);
}
