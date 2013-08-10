#ifndef BTEXTCODECCOMBOBOX_H
#define BTEXTCODECCOMBOBOX_H

class BTextCodecComboBoxPrivate;

class QTextCodec;
class QString;

#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QComboBox>

/*============================================================================
================================ BTextCodecComboBox ==========================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecComboBox : public QComboBox, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTextCodecComboBox)
public:
    explicit BTextCodecComboBox(QWidget *parent = 0);
    explicit BTextCodecComboBox(BTextCodecMenu::Style s, QWidget *parent = 0);
    ~BTextCodecComboBox();
protected:
    explicit BTextCodecComboBox(BTextCodecComboBoxPrivate &d, QWidget *parent = 0);
public:
    BTextCodecMenu::Style style() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
public Q_SLOTS:
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
protected:
    void showPopup();
    void hidePopup();
Q_SIGNALS:
    void codecChanged(QTextCodec *codec);
    void codecNameChanged(const QString &codecName);
private:
    Q_DISABLE_COPY(BTextCodecComboBox)
};

#endif // BTEXTCODECCOMBOBOX_H
