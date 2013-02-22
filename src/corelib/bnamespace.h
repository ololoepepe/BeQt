#ifndef BNAMESPACE_H
#define BNAMESPACE_H

class QUuid;
class QString;
class QProcess;
class QStringList;
class QTextCodec;
class QObject;

#include "bglobal.h"

#include <QtGlobal>
#include <QList>
#include <QPair>

/*============================================================================
================================ BeQt ========================================
============================================================================*/

namespace BeQt
{

typedef QPair<QObject *, const char *> Until;

const int Second = 1000;
const int Minute = 60 * Second;
const int Hour = 60 * Minute;
const int Kilobyte = 1024;
const int Megabyte = 1024 * Kilobyte;
const int Gigabyte = 1024 * Megabyte;

B_CORE_EXPORT void waitNonBlocking(int msecs);
B_CORE_EXPORT void waitNonBlocking(QObject *sender, const char *signal, int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(QObject *sender1, const char *signal1, QObject *sender2, const char *signal2,
                                   int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(const QList<Until> &list, int msecs = -1);
B_CORE_EXPORT Until until(QObject *object, const char *signal);
B_CORE_EXPORT QString pureUuidText(const QUuid &uuid);
B_CORE_EXPORT QString pureUuidText(const QString &uuidText);
B_CORE_EXPORT QString canonicalUuidText(const QString &uuidText);
B_CORE_EXPORT QUuid uuidFromText(const QString &uuidText);
B_CORE_EXPORT QString wrapped(const QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT QString unwrapped(const QString &text, const QString &wrappingText = "\"");
B_CORE_EXPORT void startProcess(QProcess *proc, const QString &command, const QStringList &arguments);
B_CORE_EXPORT int execProcess(const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT int execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);

}

#endif // BNAMESPACE_H
