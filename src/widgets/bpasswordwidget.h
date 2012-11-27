#ifndef BPASSWORDWIDGET_H
#define BPASSWORDWIDGET_H

class BPasswordWidgetPrivate;

class QByteArray;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QCryptographicHash>

/*============================================================================
================================ BPasswordWidget =============================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidget : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPasswordWidget)
public:
    struct PasswordWidgetData
    {
        QString password;
        QByteArray encryptedPassword;
        int charCount;
        bool save;
        bool show;
    };
public:
    explicit BPasswordWidget(QWidget *parent = 0);
    ~BPasswordWidget();
protected:
    explicit BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent = 0);
public:
    static QByteArray encrypt(const QString &string, QCryptographicHash::Algorithm method = QCryptographicHash::Sha1);
    static PasswordWidgetData stateToData(const QByteArray &ba);
    static QByteArray dataToState(const PasswordWidgetData &dt);
public:
    void setPassword(const QString &password);
    void setEncryptedPassword(const QByteArray &password, int charCount = -1);
    void setSavePassword(bool b);
    void setShowPassword(bool b);
    void setData(const PasswordWidgetData &pd);
    void clear();
    void restoreState(const QByteArray &ba);
    QString password() const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm method = QCryptographicHash::Sha1) const;
    int passwordCharCount() const;
    bool savePassword() const;
    bool showPassword() const;
    PasswordWidgetData data() const;
    PasswordWidgetData encryptedData(QCryptographicHash::Algorithm method = QCryptographicHash::Sha1) const;
    QByteArray saveState() const;
    QByteArray saveStateEncrypted(QCryptographicHash::Algorithm method = QCryptographicHash::Sha1) const;
private:
    Q_DISABLE_COPY(BPasswordWidget)
};

#endif // BPASSWORDWIDGET_H

