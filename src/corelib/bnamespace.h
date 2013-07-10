#ifndef BNAMESPACE_H
#define BNAMESPACE_H

class QUuid;
class QString;
class QProcess;
class QStringList;
class QTextCodec;
class QObject;
class QRect;
class QRectF;
class QVatiant;
class QByterray;

#include "bglobal.h"

#include <QtGlobal>
#include <QList>
#include <QPair>
#include <QSysInfo>
#include <QDataStream>

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
const QDataStream::Version DataStreamVersion = QDataStream::Qt_4_8;

B_CORE_EXPORT int area(const QRect &r);
B_CORE_EXPORT qreal area(const QRectF &r);
B_CORE_EXPORT QByteArray variantToData(const QVariant &variant);
B_CORE_EXPORT QVariant dataToVariant(const QByteArray &data);
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
B_CORE_EXPORT QByteArray serialize(const QVariant &variant, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT QVariant deserialize(const QByteArray &data, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT void startProcess(QProcess *proc, const QString &command, const QStringList &arguments);
B_CORE_EXPORT int execProcess(const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT int execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT QString translate(const char *context, const char *sourceText, const char *disambiguation = 0,
                                int n = -1);
#if defined(Q_OS_MAC)
B_CORE_EXPORT QString macVersionToString(QSysInfo::MacVersion version);
B_CORE_EXPORT QString macVersion();
#endif
#if defined(Q_OS_LINUX)
B_CORE_EXPORT QString linuxVersion();
#endif
#if defined(Q_OS_WIN)
B_CORE_EXPORT QString windowsVersionToString(QSysInfo::WinVersion version);
B_CORE_EXPORT QString windowsVersion();
#endif
B_CORE_EXPORT QString osVersion();
B_CORE_EXPORT void handleQuit(const QString &command, const QStringList &arguments);

}

#endif // BNAMESPACE_H
