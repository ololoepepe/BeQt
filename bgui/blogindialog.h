#ifndef BLOGINDIALOG_H
#define BLOGINDIALOG_H

class BPasswordWidget;

class QVBoxLayout;
class QLabel;
class QLabel;
class QFormLayout;
class QLineEdit;
class QHBoxLayout;
class QPushButton;

#include <QDialog>
#include <QString>
#include <QByteArray>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BLoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BLoginDialog(QWidget *parent = 0);
    //
    void setText(const QString &text);
    void setInformativeText(const QString &text);
    void setLogin(const QString &text);
    void setPassword(const QString &text);
    void setEncryptedPassword(const QByteArray &password, const QByteArray &key);
    void setShowPassword(bool show);
    void setSavePassword(bool save);
    QString login() const;
    QString password() const;
    QByteArray encryptedPassword(const QByteArray &key) const;
    bool showPassword() const;
    bool savePassword() const;
private:
    QVBoxLayout *_m_vlt;
      QLabel *_m_lblText;
      QLabel *_m_lblInformativeText;
      QFormLayout *_m_flt;
        QLineEdit *_m_ledtLogin;
        BPasswordWidget *_m_pwdwgt;
      QHBoxLayout *_m_hlt;
        //stretch
        QPushButton *_m_btnCancel;
        QPushButton *_m_btnOk;
};

#endif // BLOGINDIALOG_H
