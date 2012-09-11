#ifndef BTEXTEDITORSETTINGSTAB_H
#define BTEXTEDITORSETTINGSTAB_H

class BEncoding;

class QFormLayout;
class QFontComboBox;
class QSpinBox;
class QComboBox;
class QLabel;

#include "../babstractsettingstab.h"

#include <QVariantMap>
#include <QString>
#include <QList>

class BTextEditorSettingsTab : public BAbstractSettingsTab
{
    Q_OBJECT
public:
    static const QString IdFontFamily;
    static const QString IdFontPointSize;
    static const QString IdDefaultEncoding;
    static const QString IdLineLength;
    static const QString IdTabWidth;
    static const QString IdMessageTimeout;
    //
    BTextEditorSettingsTab(const QVariantMap &settings, const QList<BEncoding *> &encodings);
    //
    QVariantMap valueMap() const;
    QString title() const;
private:
    QFormLayout *_m_flt;
      QLabel *_m_lblFntcmbox;
      QFontComboBox *_m_fntcmbox;
      //
      QLabel *_m_lblFontSize;
      QSpinBox *_m_sboxFontSize;
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
      QLabel *_m_lblMessageTimeout;
      QSpinBox *_m_sboxMessageTimeout;
    //
    void _m_retranslateUi();
};

#endif // BTEXTEDITORSETTINGSTAB_H
