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
================================ Password Widget
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidget : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BPasswordWidget)
    Q_OBJECT
public:
    struct PasswordWidgetData
    {
        QString password;
        QByteArray encryptedPassword;
        int charCount;
        bool save;
        bool show;
    };
    //
    static QByteArray encrypt(const QString &string, QCryptographicHash::Algorithm method = QCryptographicHash::Sha1);
    static PasswordWidgetData stateToData(const QByteArray &ba);
    static QByteArray dataToState(const PasswordWidgetData &dt);
    //
    explicit BPasswordWidget(QWidget *parent = 0);
    ~BPasswordWidget();
    //
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
protected:
    BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BPasswordWidget)
};

#endif // BPASSWORDWIDGET_H

