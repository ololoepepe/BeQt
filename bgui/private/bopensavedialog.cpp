class QWidget;

#include "bopensavedialog.h"
#include "bencoding.h"
#include "bguicommon.h"

#include "../bcore/bcore.h"

#include <QRect>
#include <QString>
#include <QFileDialog>
#include <QList>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QVariant>
#include <QApplication>
#include <QSettings>

const QRect _m_DefGeometry = QRect(200, 200, 400, 400);
//
//_m_GroupTextEditor
  const QString _m_GroupOpenSaveDialog = "open_save_dialog";
    const QString _m_KeyGeometry = "geometry";

//

BOpenSaveDialog::BOpenSaveDialog(const QString &id, Mode mode, const QList<BEncoding *> &encodings,
                                 const QString &codecName, QWidget *parent) :
    QFileDialog(parent), _m_CId(id)
{
    setOption(DontUseNativeDialog);
    setFileMode(AnyFile);
    if (OpenMode == mode)
    {
        setWindowTitle( tr("Select file", "windowTitle") );
        setAcceptMode(AcceptOpen);
    }
    else
    {
        setWindowTitle( tr("Select file name", "windowTitle") );
        setAcceptMode(AcceptSave);
    }
    QLayout *lt = layout();
      _m_lblEncoding = new QLabel(this);
        _m_lblEncoding->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(_m_lblEncoding);
      _m_cmboxEncoding = new QComboBox(this);
        for (int i = 0; i < encodings.size(); ++i)
        {
            BEncoding *enc = encodings.at(i);
            _m_cmboxEncoding->addItem( enc->fullName(), enc->codecName() );
        }
        int index = _m_cmboxEncoding->findData(codecName);
        if (index >= 0)
            _m_cmboxEncoding->setCurrentIndex(index);
      lt->addWidget(_m_cmboxEncoding);
    connect( this, SIGNAL( accepted() ), this, SLOT( _m_saveSettings() ) );
    connect( this, SIGNAL( rejected() ), this, SLOT( _m_saveSettings() ) );
    _m_loadSettings();
}

//

QString BOpenSaveDialog::codecName() const
{
    return _m_cmboxEncoding->itemData( _m_cmboxEncoding->currentIndex() ).toString();
}

//

void BOpenSaveDialog::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupOpenSaveDialog);
        setGeometry( s->value(_m_KeyGeometry, _m_DefGeometry).toRect() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

//

void BOpenSaveDialog::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupOpenSaveDialog);
        s->setValue( _m_KeyGeometry, geometry() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}
