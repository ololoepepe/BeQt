#ifndef BTEXTEDITORSETTINGSTAB_H
#define BTEXTEDITORSETTINGSTAB_H

class QFormLayout;
class QFontComboBox;
class QSpinBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

#include "../babstractsettingstab.h"

#include <QVariantMap>
#include <QString>
#include <QMap>
#include <QStringList>

class BTextEditorSettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    static const QString IdMacrosDir;           //7
    static const QString IdDefaultEncoding;     //3
    static const QString IdFontFamily;          //1
    static const QString IdFontPointSize;       //2
    static const QString IdLineLength;          //4
    static const QString IdTabWidth;            //5
    static const QString IdKeyboardLayoutMap;   //6
    //
    BTextEditorSettingsTab(const QVariantMap &settings, const QMap<QString, QString> &encodings,
                           const QStringList &keyboardLayoutMaps);
    //
    QVariantMap valueMap() const;
    QString title() const;
private:
    QFormLayout *_m_flt;
      QLabel *_m_lblFontFamily;
      QFontComboBox *_m_fntcmboxFontFamily;
      //
      QLabel *_m_lblFontPointSize;
      QSpinBox *_m_sboxFontPointSize;
      //
      QLabel *_m_lblDefaultEncoding;
      QComboBox *_m_cmboxDefaultEncoding;
      //
      QLabel *_m_lblLineLength;
      QSpinBox *_m_sboxLineLength;
      //
      QLabel *_m_lblTabWidth;
      QComboBox *_m_cmboxTabWidth;
      //
      QLabel *_m_lblKeyboardLayoutMap;
      QComboBox *_m_cmboxKeyboardLayoutMap;
      //
      QLabel *_m_lblMacrosDir;
      //hlt
        QLineEdit *_m_ledtMacrosDir;
        QPushButton *_m_btnSearchMacrosDir;
    //
    void _m_retranslateUi();
private slots:
    void _m_btnSearchMacrosDirClicked();
};

#endif // BTEXTEDITORSETTINGSTAB_H
