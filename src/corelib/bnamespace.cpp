#include "bnamespace.h"

#include <QEventLoop>
#include <QTimer>

namespace BeQt
{

void waitNonBlocking(int msecs)
{
    if (msecs <= 0)
        return;
    QEventLoop el;
    QTimer::singleShot(msecs, &el, SLOT(quit()));
    el.exec();
}

}
