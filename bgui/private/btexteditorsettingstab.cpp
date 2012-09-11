#include "btexteditorsettingstab.h"
#include "bencoding.h"

#include <QVariantMap>
#include <QString>
#include <QFormLayout>
#include <QFontComboBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFont>
#include <QList>
#include <QLabel>

const QString BTextEditorSettingsTab::IdFontFamily = "font_family";
const QString BTextEditorSettingsTab::IdFontPointSize = "font_point_size";
const QString BTextEditorSettingsTab::IdDefaultEncoding = "default_encoding";
const QString BTextEditorSettingsTab::IdLineLength = "line_length";
const QString BTextEditorSettingsTab::IdTabWidth = "tab_width";
const QString BTextEditorSettingsTab::IdMessageTimeout = "message_timeout";

//

BTextEditorSettingsTab::BTextEditorSettingsTab(const QVariantMap &settings, const QList<BEncoding *> &encodings)
{
    _m_flt = new QFormLayout(this);
      _m_flt->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
      _m_lblFntcmbox = new QLabel;
      _m_fntcmbox = new QFontComboBox(this);
        _m_fntcmbox->setEditable(false);
        _m_fntcmbox->setFontFilters(QFontComboBox::MonospacedFonts);
        if ( settings.contains(IdFontFamily) )
        {
            QFont fnt = _m_fntcmbox->currentFont();
            fnt.setFamily( settings.value(IdFontFamily).toString() );
            _m_fntcmbox->setCurrentFont(fnt);
        }
      _m_flt->addRow(_m_lblFntcmbox, _m_fntcmbox);
      //
      _m_lblFontSize = new QLabel;
      _m_sboxFontSize = new QSpinBox(this);
        _m_sboxFontSize->setMinimum(6);
        _m_sboxFontSize->setMaximum(100);
        if ( settings.contains(IdFontPointSize) )
            _m_sboxFontSize->setValue( settings.value(IdFontPointSize).toInt() );
        else
            _m_sboxFontSize->setValue(10);
      _m_flt->addRow(_m_lblFontSize, _m_sboxFontSize);
      //
      _m_lblDefaultEncoding = new QLabel;
      _m_cmboxDefaultEncoding = new QComboBox(this);
        for (int i = 0; i < encodings.size(); ++i)
        {
            BEncoding *enc = encodings.at(i);
            _m_cmboxDefaultEncoding->addItem( enc->fullName(), enc->codecName() );
        }
        if ( settings.contains(IdDefaultEncoding) )
        {
            int ind = _m_cmboxDefaultEncoding->findData( settings.value(IdDefaultEncoding).toString() );
            if (ind >= 0)
                _m_cmboxDefaultEncoding->setCurrentIndex(ind);
        }
      _m_flt->addRow(_m_lblDefaultEncoding, _m_cmboxDefaultEncoding);
      //
      _m_lblLineLength = new QLabel;
      _m_sboxLineLength = new QSpinBox(this);
        _m_sboxLineLength->setMinimum(20);
        _m_sboxLineLength->setMaximum(500);
        if ( settings.contains(IdLineLength) )
            _m_sboxLineLength->setValue( settings.value(IdLineLength).toInt() );
        else
            _m_sboxLineLength->setValue(120);
      _m_flt->addRow(_m_lblLineLength, _m_sboxLineLength);
      _m_lblTabWidth = new QLabel;
      _m_cmboxTabWidth = new QComboBox(this);
        _m_cmboxTabWidth->addItem("2", (int) 2);
        _m_cmboxTabWidth->addItem("4", (int) 4);
        _m_cmboxTabWidth->addItem("8", (int) 8);
        if ( settings.contains(IdTabWidth) )
            _m_cmboxTabWidth->setCurrentIndex( _m_cmboxTabWidth->findData( settings.value(IdTabWidth).toInt() ) );
        else
            _m_cmboxTabWidth->setCurrentIndex(2);
      _m_flt->addRow(_m_lblTabWidth, _m_cmboxTabWidth);
      _m_lblMessageTimeout = new QLabel(this);
      _m_sboxMessageTimeout = new QSpinBox(this);
        _m_sboxMessageTimeout->setMinimum(1);
        _m_sboxMessageTimeout->setMaximum(30);
        _m_sboxMessageTimeout->setValue(settings.value(IdMessageTimeout, 15000).toInt() / 1000);
      _m_flt->addRow(_m_lblMessageTimeout, _m_sboxMessageTimeout);
    //
    _m_retranslateUi();
}

//

QVariantMap BTextEditorSettingsTab::valueMap() const
{
    QVariantMap m;
    m.insert( IdFontFamily, _m_fntcmbox->currentFont().family() );
    m.insert( IdFontPointSize, _m_sboxFontSize->value() );
    QString enc = _m_cmboxDefaultEncoding->itemData( _m_cmboxDefaultEncoding->currentIndex() ).toString();
    m.insert(IdDefaultEncoding, enc.isEmpty() ? "UTF-8" : enc);
    m.insert( IdLineLength, _m_sboxLineLength->value() );
    m.insert( IdTabWidth, _m_cmboxTabWidth->itemData( _m_cmboxTabWidth->currentIndex() ).toInt() );
    m.insert(IdMessageTimeout, _m_sboxMessageTimeout->value() * 1000);
    return m;
}

QString BTextEditorSettingsTab::title() const
{
    return tr("Text editor", "settingsTab title");
}

//

void BTextEditorSettingsTab::_m_retranslateUi()
{
    _m_lblFntcmbox->setText( tr("Font:", "lbl text") );
    _m_lblFontSize->setText( tr("Font size:", "lbl text") );
    _m_lblDefaultEncoding->setText( tr("Default file encoding:", "lbl text") );
    _m_lblDefaultEncoding->setToolTip( tr("All files will be opened using this encoding by default. "
                                          "Note: you may reopen files using another encoding", "lbl toolTip") );
    _m_lblLineLength->setText( tr("Line length:", "lbl text") );
    _m_lblLineLength->setToolTip( tr("Maximum line length for documents. "
                                     "If opened file has longer lines, these lines will be breaked into several ones",
                                     "lbl text") );
    _m_lblTabWidth->setText( tr("Tab width:", "lbl text") );
    _m_lblTabWidth->setToolTip( tr("Number of spaces inserted on <Tab> key press", "lbl toolTip") );
    _m_lblMessageTimeout->setText( tr("Message timeout:", "lbl text") );
    _m_lblMessageTimeout->setToolTip( tr("Sets for how long status bar messages are shown (in seconds)",
                                         "lbl toolTip") );
}
