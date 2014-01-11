#ifndef BPASSWORDGROUP_H
#define BPASSWORDGROUP_H

class BPasswordGroupPrivate;
class BPasswordWidget;
class BPassword;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BPasswordGroup ==============================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordGroup : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPasswordGroup)
public:
    explicit BPasswordGroup(QObject *parent = 0);
    ~BPasswordGroup();
protected:
    explicit BPasswordGroup(BPasswordGroupPrivate &d, QObject *parent = 0);
public:
    void addPasswordWidget(BPasswordWidget *pwdwgt);
    bool passwordsMatch() const;
    BPassword password() const;
Q_SIGNALS:
    void passwordsMatchChanged(bool match);
private:
    Q_DISABLE_COPY(BPasswordGroup)
};

#endif // BPASSWORDGROUP_H
