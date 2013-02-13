#include "bnamespace.h"

#include <QEventLoop>
#include <QTimer>
#include <QUuid>
#include <QString>

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

QString pureUuidText(const QUuid &uuid)
{
    QString t = uuid.toString();
    return t.mid(1, t.length() - 2);
}

QString pureUuidText(const QString &uuidText)
{
    return pureUuidText(QUuid(canonicalUuidText(uuidText)));
}

QString canonicalUuidText(const QString &uuidText)
{
    if (uuidText.isEmpty())
        return "";
    QString t = (uuidText.at(0) != '{' ? "{" : "") + uuidText + (uuidText.at(uuidText.length() - 1) != '}' ? "}" : "");
    return !QUuid(t).isNull() ? t : QString();
}

QUuid uuidFromText(const QString &uuidText)
{
    return QUuid(canonicalUuidText(uuidText));
}

}
