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

enum TabWidth
{
    TabWidth2 = 2,
    TabWidth4 = 4,
    TabWidth8 = 8
};

enum CodecsGroup
{
    InvalidGroup = 0,
    UnicodeGroup,
    EastEuropeanGroup,
    WestEuropeanGroup,
    EastAsianGroup,
    SouthEastSouthWestAsianGroup,
    MiddleEastGroup
};

enum FileSizeFormat
{
    BytesFormat,
    KilobytesFormat,
    MegabytesFormat,
    GigabytesFormat
};

enum OSType
{
    UnknownOS,
    LinuxOS,
    MacOS,
    WindowsOS
};

typedef QPair<const QObject *, const char *> Until;
typedef QPair<const QObject *, const char *> Target;

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
B_CORE_EXPORT void waitNonBlocking(const QObject *sender, const char *signal, int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(const QObject *sender1, const char *signal1, const QObject *sender2,
                                   const char *signal2, int msecs = -1);
B_CORE_EXPORT void waitNonBlocking(const QList<Until> &list, int msecs = -1);
B_CORE_EXPORT Until until(const QObject *object, const char *signal);
B_CORE_EXPORT Target target(const QObject *object, const char *method);
B_CORE_EXPORT QString pureUuidText(const QUuid &uuid);
B_CORE_EXPORT QString pureUuidText(const QString &uuidText);
B_CORE_EXPORT QString canonicalUuidText(const QString &uuidText);
B_CORE_EXPORT QUuid uuidFromText(const QString &uuidText);
B_CORE_EXPORT QByteArray serialize(const QVariant &variant, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT QVariant deserialize(const QByteArray &data, QDataStream::Version version = DataStreamVersion);
B_CORE_EXPORT void startProcess(QProcess *proc, const QString &command, const QStringList &arguments);
B_CORE_EXPORT int execProcess(const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT int execProcess(const QString &workingDir, const QString &command, const QStringList &arguments,
                              int startTimeout, int finishTimeout, QString *output = 0, QTextCodec *codec = 0);
B_CORE_EXPORT QString translate(const char *context, const char *sourceText, const char *disambiguation = 0,
                                int n = -1);
B_CORE_EXPORT bool isCodecSupported(QTextCodec *codec);
B_CORE_EXPORT bool isCodecSupported(const QString &codecName);
B_CORE_EXPORT QList<QTextCodec *> supportedCodecs();
B_CORE_EXPORT QStringList supportedCodecsNames();
B_CORE_EXPORT QString codecName(QTextCodec *codec);
B_CORE_EXPORT QString codecName(const QByteArray &cn);
B_CORE_EXPORT QString fullCodecName(QTextCodec *codec);
B_CORE_EXPORT QString fullCodecName(const QString &codecName);
B_CORE_EXPORT QTextCodec *codec(const QString &cn);
B_CORE_EXPORT QTextCodec *codec(const QByteArray &cn);
B_CORE_EXPORT QList<CodecsGroup> codecsGroups();
B_CORE_EXPORT QString codecsGroupName(CodecsGroup group);
B_CORE_EXPORT QList<QTextCodec *> codecsForGroup(CodecsGroup group);
B_CORE_EXPORT QStringList codecNamesForGroup(CodecsGroup group);
B_CORE_EXPORT CodecsGroup codecsGroupFromInt(int cg);
B_CORE_EXPORT TabWidth tabWidthFromInt(int tw);
B_CORE_EXPORT QString fileSizeToString(qint64 size, FileSizeFormat format = BytesFormat, quint8 precision = 1);
B_CORE_EXPORT QString fileSizeToStringNoTr(qint64 size, FileSizeFormat format = BytesFormat, quint8 precision = 1);
B_CORE_EXPORT void takeOwnership(QObject *object, QObject *newOwner);
B_CORE_EXPORT void deleteUnowned(QObject *object, QObject *possibleOwner = 0);
B_CORE_EXPORT void deleteLaterUnowned(QObject *object, QObject *possibleOwner = 0);

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
B_CORE_EXPORT OSType osType();
B_CORE_EXPORT QString osTypeToString(OSType t);

}

#endif // BNAMESPACE_H
