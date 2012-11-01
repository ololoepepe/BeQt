#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;

#include <BeQt>

#include <QObject>

class B_CORE_EXPORT BCoreApplication : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BCoreApplication)
public:
    BCoreApplication();
    ~BCoreApplication();
private:
    Q_DISABLE_COPY(BCoreApplication)
};

#endif // BCOREAPPLICATION_H
