#ifndef BEDITGROUP_H
#define BEDITGROUP_H

class BEditGroupPrivate;

class QLineEdit;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BEditGroup ==================================
============================================================================*/

class B_WIDGETS_EXPORT BEditGroup : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BEditGroup)
public:
    explicit BEditGroup(QObject *parent = 0);
    ~BEditGroup();
protected:
    explicit BEditGroup(BEditGroupPrivate &d, QObject *parent = 0);
public:
    void addEdit(QLineEdit *ledt);
    bool textsMatch() const;
    QString text() const;
Q_SIGNALS:
    void textsMatchChanged(bool match);
private:
    Q_DISABLE_COPY(BEditGroup)
};

#endif // BEDITGROUP_H
