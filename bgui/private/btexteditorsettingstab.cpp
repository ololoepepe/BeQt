#include "btexteditorsettingstab.h"
#include "btexteditordocument.h"

#include <QVariantMap>
#include <QString>
#include <QFormLayout>
#include <QFontComboBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFont>
#include <QList>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

const QString BTextEditorSettingsTab::IdToolBarIconSize = "toolbar_icon_size";
const QString BTextEditorSettingsTab::IdFontFamily = "font_family";
const QString BTextEditorSettingsTab::IdFontPointSize = "font_point_size";
const QString BTextEditorSettingsTab::IdDefaultEncoding = "default_encoding";
const QString BTextEditorSettingsTab::IdLineLength = "line_length";
const QString BTextEditorSettingsTab::IdTabWidth = "tab_width";
const QString BTextEditorSettingsTab::IdKeyboardLayoutMap = "keyboard_layout_map";
const QString BTextEditorSettingsTab::IdMacrosDir = "macros_dir";
//
const int BTextEditorSettingsTab::ToolBarIconSizeMin = 16;
const int BTextEditorSettingsTab::ToolBarIconSizeDef = 22;
const int BTextEditorSettingsTab::ToolBarIconSizeMax = 48;

//

bool BTextEditorSettingsTab::checkToolBarIconSize(int size)
{
    return size >= ToolBarIconSizeMin && size <= ToolBarIconSizeMax;
}

//

const QString BTextEditorSettingsTab::_m_KeyboardLayoutMapDef = "---";

//

BTextEditorSettingsTab::BTextEditorSettingsTab(const QVariantMap &settings, const QMap<QString, QString> &encodings,
                                               const QStringList &keyboardLayoutMaps)
{
    _m_flt = new QFormLayout(this);
      //toolbar icon size
      if ( settings.contains(IdToolBarIconSize) )
      {
          _m_lblToolBarIconSize = new QLabel(this);
          _m_sboxToolBarIconSize = new QSpinBox(this);
            _m_sboxToolBarIconSize->setMinimum(ToolBarIconSizeMin);
            _m_sboxToolBarIconSize->setMaximum(ToolBarIconSizeMax);
            _m_sboxToolBarIconSize->setValue( settings.value(IdToolBarIconSize).toInt() );
          _m_flt->addRow(_m_lblToolBarIconSize, _m_sboxToolBarIconSize);
      }
      else
      {
          _m_lblToolBarIconSize = 0;
          _m_sboxToolBarIconSize = 0;
      }
      //font family
      if ( settings.contains(IdFontFamily) )
      {
          _m_lblFontFamily = new QLabel(this);
          _m_fntcmboxFontFamily = new QFontComboBox(this);
            _m_fntcmboxFontFamily->setEditable(false);
            _m_fntcmboxFontFamily->setFontFilters(QFontComboBox::MonospacedFonts);
            QFont fnt = _m_fntcmboxFontFamily->currentFont();
            fnt.setFamily( settings.value(IdFontFamily).toString() );
            _m_fntcmboxFontFamily->setCurrentFont(fnt);
          _m_flt->addRow(_m_lblFontFamily, _m_fntcmboxFontFamily);
      }
      else
      {
          _m_lblFontFamily = 0;
          _m_fntcmboxFontFamily = 0;
      }
      //font point size
      if ( settings.contains(IdFontPointSize) )
      {
          _m_lblFontPointSize = new QLabel(this);
          _m_sboxFontPointSize = new QSpinBox(this);
            _m_sboxFontPointSize->setMinimum(BTextEditorDocument::FontPointSizeMin);
            _m_sboxFontPointSize->setMaximum(BTextEditorDocument::FontPointSizeMax);
            _m_sboxFontPointSize->setValue( settings.value(IdFontPointSize).toInt() );
          _m_flt->addRow(_m_lblFontPointSize, _m_sboxFontPointSize);
      }
      else
      {
          _m_lblFontPointSize = 0;
          _m_sboxFontPointSize = 0;
      }
      //default encoding
      if ( settings.contains(IdDefaultEncoding) )
      {
          _m_lblDefaultEncoding = new QLabel(this);
          _m_cmboxDefaultEncoding = new QComboBox(this);
            QStringList sl = encodings.keys();
            for (int i = 0; i < sl.size(); ++i)
            {
                const QString &cn = sl.at(i);
                _m_cmboxDefaultEncoding->addItem(encodings.value(cn), cn);
            }
            _m_cmboxDefaultEncoding->setCurrentIndex( _m_cmboxDefaultEncoding->findData(
                                                          settings.value(IdDefaultEncoding).toString() ) );
          _m_flt->addRow(_m_lblDefaultEncoding, _m_cmboxDefaultEncoding);
      }
      else
      {
          _m_lblDefaultEncoding = 0;
          _m_cmboxDefaultEncoding = 0;
      }
      //line length
      if ( settings.contains(IdLineLength) )
      {
          _m_lblLineLength = new QLabel(this);
          _m_sboxLineLength = new QSpinBox(this);
            _m_sboxLineLength->setMinimum(BTextEditorDocument::LineLengthMin);
            _m_sboxLineLength->setMaximum(BTextEditorDocument::LineLengthMax);
            _m_sboxLineLength->setValue( settings.value(IdLineLength).toInt() );
          _m_flt->addRow(_m_lblLineLength, _m_sboxLineLength);
      }
      else
      {
          _m_lblLineLength = 0;
          _m_sboxLineLength = 0;
      }
      //tab width
      if ( settings.contains(IdTabWidth) )
      {
          _m_lblTabWidth = new QLabel(this);
          _m_cmboxTabWidth = new QComboBox(this);
            for (int i = 0; i < BTextEditorDocument::TabWidthsValid.size(); ++i)
            {
                int tw = BTextEditorDocument::TabWidthsValid.at(i);
                _m_cmboxTabWidth->addItem(QString::number(tw), tw);
            }
            _m_cmboxTabWidth->setCurrentIndex( _m_cmboxTabWidth->findData( settings.value(IdTabWidth).toInt() ) );
          _m_flt->addRow(_m_lblTabWidth, _m_cmboxTabWidth);
      }
      else
      {
          _m_lblTabWidth = 0;
          _m_cmboxTabWidth = 0;
      }
      //keyboard layout map
      if ( settings.contains(IdKeyboardLayoutMap) )
      {
          _m_lblKeyboardLayoutMap = new QLabel(this);
          _m_cmboxKeyboardLayoutMap = new QComboBox(this);
            _m_cmboxKeyboardLayoutMap->addItem(_m_KeyboardLayoutMapDef);
            _m_cmboxKeyboardLayoutMap->addItems(keyboardLayoutMaps);
            int ind = _m_cmboxKeyboardLayoutMap->findText( settings.value(IdKeyboardLayoutMap).toString() );
            _m_cmboxKeyboardLayoutMap->setCurrentIndex(
                        ind >= 0 ? ind : _m_cmboxKeyboardLayoutMap->findText(_m_KeyboardLayoutMapDef) );
          _m_flt->addRow(_m_lblKeyboardLayoutMap, _m_cmboxKeyboardLayoutMap);
      }
      else
      {
          _m_lblKeyboardLayoutMap = 0;
          _m_cmboxKeyboardLayoutMap = 0;
      }
      //macros dir
      if ( settings.contains(IdMacrosDir) )
      {
          _m_lblMacrosDir = new QLabel(this);
          QHBoxLayout *hlt = new QHBoxLayout;
            _m_ledtMacrosDir = new QLineEdit(this);
              _m_ledtMacrosDir->setReadOnly(true);
              _m_ledtMacrosDir->setText( settings.value(IdMacrosDir).toString() );
            hlt->addWidget(_m_ledtMacrosDir);
            _m_btnSearchMacrosDir = new QPushButton(this);
              _m_btnSearchMacrosDir->setText( tr("Search...", "btn text") );
              connect( _m_btnSearchMacrosDir, SIGNAL( clicked() ), this, SLOT( _m_btnSearchMacrosDirClicked() ) );
            hlt->addWidget(_m_btnSearchMacrosDir);
          _m_flt->addRow(_m_lblMacrosDir, hlt);
      }
      else
      {
          _m_lblMacrosDir = 0;
          _m_ledtMacrosDir = 0;
          _m_btnSearchMacrosDir = 0;
      }
    //
    _m_retranslateUi();
}

//

QVariantMap BTextEditorSettingsTab::valueMap() const
{
    QVariantMap m;
    if (_m_sboxToolBarIconSize)
        m.insert( IdToolBarIconSize, _m_sboxToolBarIconSize->value() );
    if (_m_ledtMacrosDir)
        m.insert( IdMacrosDir, _m_ledtMacrosDir->text() );
    if (_m_cmboxDefaultEncoding)
        m.insert( IdDefaultEncoding, _m_cmboxDefaultEncoding->itemData( _m_cmboxDefaultEncoding->currentIndex() ) );
    if (_m_fntcmboxFontFamily)
        m.insert( IdFontFamily, _m_fntcmboxFontFamily->currentFont().family() );
    if (_m_sboxFontPointSize)
        m.insert( IdFontPointSize, _m_sboxFontPointSize->value() );
    if (_m_sboxLineLength)
        m.insert( IdLineLength, _m_sboxLineLength->value() );
    if (_m_cmboxTabWidth)
        m.insert( IdTabWidth, _m_cmboxTabWidth->itemData( _m_cmboxTabWidth->currentIndex() ).toInt() );
    if (_m_cmboxKeyboardLayoutMap)
        m.insert( IdKeyboardLayoutMap, _m_cmboxKeyboardLayoutMap->currentText() );
    return m;
}

QString BTextEditorSettingsTab::title() const
{
    return tr("Text editor", "settingsTab title");
}

//

void BTextEditorSettingsTab::_m_retranslateUi()
{
    if (_m_lblToolBarIconSize)
        _m_lblToolBarIconSize->setText( tr("Toolbar icon size:", "lbl text") );
    if (_m_lblFontFamily)
        _m_lblFontFamily->setText( tr("Font:", "lbl text") );
    if (_m_lblFontPointSize)
        _m_lblFontPointSize->setText( tr("Font size:", "lbl text") );
    if (_m_lblDefaultEncoding)
    {
        _m_lblDefaultEncoding->setText( tr("Default file encoding:", "lbl text") );
        _m_lblDefaultEncoding->setToolTip( tr("All files will be opened using this encoding by default. "
                                              "Note: you may reopen files using another encoding", "lbl toolTip") );
    }
    if (_m_lblLineLength)
    {
        _m_lblLineLength->setText( tr("Line length:", "lbl text") );
        _m_lblLineLength->setToolTip( tr("Maximum line length for documents. "
                                         "If opened file has longer lines, these lines will be splitted", "lbl text") );
    }
    if (_m_lblTabWidth)
    {
        _m_lblTabWidth->setText( tr("Tab width:", "lbl text") );
        _m_lblTabWidth->setToolTip( tr("Number of spaces inserted on <Tab> key press", "lbl toolTip") );
    }
    if (_m_lblKeyboardLayoutMap)
        _m_lblKeyboardLayoutMap->setText( tr("Keyboard layout map:", "lbl text") );
    if (_m_lblMacrosDir)
        _m_lblMacrosDir->setText( tr("Macros directory:", "lbl text") );
}

//

void BTextEditorSettingsTab::_m_btnSearchMacrosDirClicked()
{
    QString dir = QFileDialog::getExistingDirectory( this, tr("Select directory", "fileDialog caption"),
                                                     _m_ledtMacrosDir->text() );
    if ( dir.isEmpty() )
        return;
    _m_ledtMacrosDir->setText(dir);
}
