#ifndef BPASSWORDWIDGET_H
#define BPASSWORDWIDGET_H

class BPasswordWidgetPrivate;

class QByteArray;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BPassword>

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
    typedef QString (*GeneratePasswordFunction)(int);
public:
    explicit BPasswordWidget(QWidget *parent = 0);
    explicit BPasswordWidget(const BPassword &password, QWidget *parent = 0);
    ~BPasswordWidget();
protected:
    explicit BPasswordWidget(BPasswordWidgetPrivate &d, QWidget *parent = 0);
public:
    void setMode(BPassword::Mode mode);
    void setPassword(const BPassword &password);
    void setPassword(const QString &password);
    void setPassword(QCryptographicHash::Algorithm a, const QByteArray &password, int charCount = -1);
    void encrypt(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void setSavePasswordVisible(bool visible);
    void setShowPasswordVisible(bool visible);
    void setGeneratePasswordVisible(bool visible);
    void setGeneratePasswordFunction(GeneratePasswordFunction f);
    void setGeneratedPasswordLength(int len);
    void clear();
    void restorePasswordState(const QByteArray &ba);
    void restoreWidgetState(const QByteArray &ba);
    void restoreState(const QByteArray &ba);
    BPassword::Mode mode() const;
    BPassword password() const;
    QString openPassword() const;
    QByteArray encryptedPassword(int *charCountHint = 0) const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint = 0) const;
    int charCountHint(BPassword::Mode mode = BPassword::FlexibleMode) const;
    QCryptographicHash::Algorithm algorithm() const;
    bool savePassword() const;
    bool showPassword() const;
    bool savePasswordVisible() const;
    bool showPasswordVisible() const;
    bool generatePasswordVisible() const;
    GeneratePasswordFunction generatePasswordFunction() const;
    int generatedPasswordLength() const;
    QByteArray savePasswordState(BPassword::Mode mode = BPassword::FlexibleMode) const;
    QByteArray saveWidgetState() const;
    QByteArray saveState(BPassword::Mode mode = BPassword::FlexibleMode) const;
public Q_SLOTS:
    void setSavePassword(bool b);
    void setShowPassword(bool b);
    void generatePassword();
Q_SIGNALS:
    void savePasswordChanged(bool b);
    void showPasswordChanged(bool b);
    void passwordChanged();
    void passwordChanged(const QString &password);
    void passwordChanged(const QByteArray &encryptedPassword);
    void passwordEdited(const QString &password);
    void returnPressed();
    void passwordGenerated(const QString &password);
private:
    Q_DISABLE_COPY(BPasswordWidget)
};

#endif // BPASSWORDWIDGET_H
