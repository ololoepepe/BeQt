#ifndef BLOCALECOMBOBOX_H
#define BLOCALECOMBOBOX_H

class BLocaleComboBoxPrivate;

class QWidget;
class QLocale;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QComboBox>

/*============================================================================
================================ Locale Combo Box
============================================================================*/

class B_WIDGETS_EXPORT BLocaleComboBox : public QComboBox, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocaleComboBox)
public:
    explicit BLocaleComboBox(QWidget *parent = 0);
    explicit BLocaleComboBox(bool alwaysIncludeEnglish, QWidget *parent = 0);
    ~BLocaleComboBox();
    //
    QLocale currentLocale() const;
public slots:
    void setCurrentLocale(const QLocale &locale);
    void updateAvailableLocales();
signals:
    void currentLocaleChanged(const QLocale &locale);
protected:
    BLocaleComboBox(BLocaleComboBoxPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BLocaleComboBox)
};

#endif // BLOCALECOMBOBOX_H
