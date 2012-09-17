#ifndef BPASSWORDWIDGET_H
#define BPASSWORDWIDGET_H

class QHBoxLayout;
class QLineEdit;
class QCheckBox;

#include <QWidget>
#include <QString>
#include <QByteArray>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BPasswordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BPasswordWidget(QWidget *parent = 0);
    //
    void setPassword(const QString &text);
    void setEncryptedPassword(const QByteArray &password, const QByteArray &key);
    void setShowPassword(bool show);
    void setSavePassword(bool save);
    QString password() const;
    QByteArray encryptedPassword(const QByteArray &key) const;
    bool showPassword() const;
    bool savePassword() const;
private:
    QHBoxLayout *_m_hlt;
      QLineEdit *_m_ledt;
      QCheckBox *_m_cboxShow;
      QCheckBox *_m_cboxSave;
private slots:
    void _m_retranslateUi();
    void _m_cboxShowToggled(bool b);
};

#endif // BPASSWORDWIDGET_H
