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

const QString KeyboardLayoutMapDef = "---";

//

const QString BTextEditorSettingsTab::IdMacrosDir = "macros_dir";
const QString BTextEditorSettingsTab::IdDefaultEncoding = "default_encoding";
const QString BTextEditorSettingsTab::IdFontFamily = "font_family";
const QString BTextEditorSettingsTab::IdFontPointSize = "font_point_size";
const QString BTextEditorSettingsTab::IdLineLength = "line_length";
const QString BTextEditorSettingsTab::IdTabWidth = "tab_width";
const QString BTextEditorSettingsTab::IdKeyboardLayoutMap = "keyboard_layout_map";

//

BTextEditorSettingsTab::BTextEditorSettingsTab(const QVariantMap &settings, const QMap<QString, QString> &encodings,
                                               const QStringList &keyboardLayoutMaps)
{
    _m_flt = new QFormLayout(this);
      //_m_flt->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
      //font family
      _m_lblFontFamily = new QLabel(this);
      _m_fntcmboxFontFamily = new QFontComboBox(this);
        _m_fntcmboxFontFamily->setEditable(false);
        _m_fntcmboxFontFamily->setFontFilters(QFontComboBox::MonospacedFonts);
        QFont fnt = _m_fntcmboxFontFamily->currentFont();
        fnt.setFamily( settings.value(IdFontFamily).toString() );
        _m_fntcmboxFontFamily->setCurrentFont(fnt);
      _m_flt->addRow(_m_lblFontFamily, _m_fntcmboxFontFamily);
      //font point size
      _m_lblFontPointSize = new QLabel(this);
      _m_sboxFontPointSize = new QSpinBox(this);
        _m_sboxFontPointSize->setMinimum(BTextEditorDocument::FontPointSizeMin);
        _m_sboxFontPointSize->setMaximum(BTextEditorDocument::FontPointSizeMax);
        _m_sboxFontPointSize->setValue( settings.value(IdFontPointSize).toInt() );
      _m_flt->addRow(_m_lblFontPointSize, _m_sboxFontPointSize);
      //default encoding
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
      //line length
      _m_lblLineLength = new QLabel(this);
      _m_sboxLineLength = new QSpinBox(this);
        _m_sboxLineLength->setMinimum(BTextEditorDocument::LineLengthMin);
        _m_sboxLineLength->setMaximum(BTextEditorDocument::LineLengthMax);
        _m_sboxLineLength->setValue( settings.value(IdLineLength).toInt() );
      _m_flt->addRow(_m_lblLineLength, _m_sboxLineLength);
      _m_lblTabWidth = new QLabel(this);
      _m_cmboxTabWidth = new QComboBox(this);
        for (int i = 0; i < BTextEditorDocument::TabWidthsValid.size(); ++i)
        {
            int tw = BTextEditorDocument::TabWidthsValid.at(i);
            _m_cmboxTabWidth->addItem(QString::number(tw), tw);
        }
        _m_cmboxTabWidth->setCurrentIndex( _m_cmboxTabWidth->findData( settings.value(IdTabWidth).toInt() ) );
      _m_flt->addRow(_m_lblTabWidth, _m_cmboxTabWidth);
      //keyboard layout map
      _m_lblKeyboardLayoutMap = new QLabel(this);
      _m_cmboxKeyboardLayoutMap = new QComboBox(this);
        _m_cmboxKeyboardLayoutMap->addItem(KeyboardLayoutMapDef);
        _m_cmboxKeyboardLayoutMap->addItems(keyboardLayoutMaps);
        int ind = _m_cmboxKeyboardLayoutMap->findText( settings.value(IdKeyboardLayoutMap).toString() );
        _m_cmboxKeyboardLayoutMap->setCurrentIndex(
                    ind >= 0 ? ind : _m_cmboxKeyboardLayoutMap->findText(KeyboardLayoutMapDef) );
      _m_flt->addRow(_m_lblKeyboardLayoutMap, _m_cmboxKeyboardLayoutMap);
      //macros dir
      _m_lblMacrosDir = new QLabel(this);
      QHBoxLayout *hlt = new QHBoxLayout;
        _m_ledtMacrosDir = new QLineEdit(this);
          _m_ledtMacrosDir->setReadOnly(true);
          _m_ledtMacrosDir->setText( settings.value(IdMacrosDir).toString() );
        hlt->addWidget(_m_ledtMacrosDir);
        _m_btnSearchMacrosDir = new QPushButton(this);
          connect( _m_btnSearchMacrosDir, SIGNAL( clicked() ), this, SLOT( _m_btnSearchMacrosDirClicked() ) );
        hlt->addWidget(_m_btnSearchMacrosDir);
      _m_flt->addRow(_m_lblMacrosDir, hlt);
    //
    _m_retranslateUi();
}

//

QVariantMap BTextEditorSettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( IdMacrosDir, _m_ledtMacrosDir->text() );
    m.insert( IdDefaultEncoding, _m_cmboxDefaultEncoding->itemData( _m_cmboxDefaultEncoding->currentIndex() ) );
    m.insert( IdFontFamily, _m_fntcmboxFontFamily->currentFont().family() );
    m.insert( IdFontPointSize, _m_sboxFontPointSize->value() );
    m.insert( IdLineLength, _m_sboxLineLength->value() );
    m.insert( IdTabWidth, _m_cmboxTabWidth->itemData( _m_cmboxTabWidth->currentIndex() ).toInt() );
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
    _m_lblFontFamily->setText( tr("Font:", "lbl text") );
    _m_lblFontPointSize->setText( tr("Font size:", "lbl text") );
    _m_lblDefaultEncoding->setText( tr("Default file encoding:", "lbl text") );
    _m_lblDefaultEncoding->setToolTip( tr("All files will be opened using this encoding by default. "
                                          "Note: you may reopen files using another encoding", "lbl toolTip") );
    _m_lblLineLength->setText( tr("Line length:", "lbl text") );
    _m_lblLineLength->setToolTip( tr("Maximum line length for documents. "
                                     "If opened file has longer lines, these lines will be splitted", "lbl text") );
    _m_lblTabWidth->setText( tr("Tab width:", "lbl text") );
    _m_lblTabWidth->setToolTip( tr("Number of spaces inserted on <Tab> key press", "lbl toolTip") );
    _m_lblKeyboardLayoutMap->setText( tr("Keyboard layout map:", "lbl text") );
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
