#ifndef BPASSWORDWIDGET_H
#define BPASSWORDWIDGET_H

class BPasswordWidgetPrivate;

class QByteArray;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QCryptographicHash>

class B_WIDGETS_EXPORT BPasswordWidget : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BPasswordWidget)
    Q_OBJECT
public:
    static QByteArray encrypt(const QString &string, QCryptographicHash::Algorithm method = QCryptographicHash::Sha1);
    //
    explicit BPasswordWidget(QWidget *parent = 0);
    ~BPasswordWidget();
    //
    void setPassword(const QString &password);
    void setEncryptedPassword(const QByteArray &password, int charCount = -1);
    void setSavePassword(bool b);
    void setShowPassword(bool b);
    void clear();
    QString password() const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm method = QCryptographicHash::Sha1) const;
    int passwordCharCount() const;
    bool savePassword() const;
    bool showPassword() const;
protected:
    BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BPasswordWidget)
};

#endif // BPASSWORDWIDGET_H

