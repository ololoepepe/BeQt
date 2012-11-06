#ifndef BLOCALECOMBOBOX_H
#define BLOCALECOMBOBOX_H

class BLocaleComboBoxPrivate;

class QWidget;
class QLocale;

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase.h>

#include <QComboBox>

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
    BLocaleComboBox(BLocaleComboBoxPrivate &d);
private:
    Q_DISABLE_COPY(BLocaleComboBox)
};

#endif // BLOCALECOMBOBOX_H
