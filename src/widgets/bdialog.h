#ifndef BDIALOG_H
#define BDIALOG_H

class BDialogPrivate;

class QWidget;
class QDialogButtonBox;
class QAbstractButton;
class QPushButton;
class QString;

#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>

#include <QDialog>
#include <QDialogButtonBox>
#include <QList>

/*============================================================================
================================ BDialog =====================================
============================================================================*/

class B_WIDGETS_EXPORT BDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BDialog)
public:
    explicit BDialog(QWidget *parent = 0);
    explicit BDialog(QDialogButtonBox::StandardButtons buttons, QWidget *parent = 0);
    ~BDialog();
protected:
    explicit BDialog(BDialogPrivate &d, QWidget *parent = 0);
public:
    void setWidget(QWidget *w);
    void setStretchEnabled(bool b);
    void setCenterButtons(bool center);
    void setStandardButtons(QDialogButtonBox::StandardButtons buttons);
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const char *method = 0);
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QObject *object,
                   const char *method);
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const char *method = 0);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QObject *object,
                           const char *method);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const char *method = 0);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const QObject *object, const char *method);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const QList<BeQt::Target> &targets);
    void removeButton(QAbstractButton *button);
    QWidget *widget() const;
    bool stretchEnabled() const;
    QDialogButtonBox *buttonBox() const;
    bool centerButtons() const;
    QDialogButtonBox::StandardButton standardButton(QAbstractButton *button) const;
    QDialogButtonBox::StandardButtons standardButtons() const;
    QPushButton *button(QDialogButtonBox::StandardButton which) const;
    QDialogButtonBox::ButtonRole buttonRole(QAbstractButton *button) const;
    QList<QAbstractButton *> buttons() const;
private:
    Q_DISABLE_COPY(BDialog)
};

#endif // BDIALOG_H
