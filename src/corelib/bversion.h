#ifndef BVERSION_H
#define BVERSION_H

class BVersionPrivate;

class QDataStream;
class QDebug;
class QVariant;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QChar>
#include <QMetaType>

#if defined(major)
#undef major
#endif

#if defined(minor)
#undef minor
#endif

/*============================================================================
================================ BVersion ====================================
============================================================================*/

class B_CORE_EXPORT BVersion : public BBase
{
    B_DECLARE_PRIVATE(BVersion)
public:
    enum Status
    {
        NoStatus = 0,
        PreAlpha,
        Alpha,
        Beta,
        ReleaseCandidate
    };
    enum StatusRepresentation
    {
        ShortLowercase,
        ShortUppercase,
        Full
    };
public:
    static QString statusToString(Status s, StatusRepresentation r = ShortLowercase);
    static int compare(const BVersion &v1, const BVersion &v2);
public:
    explicit BVersion(qint8 major = -1, qint8 minor = -1, qint8 patch = -1, Status s = NoStatus, qint8 extra = -1);
    explicit BVersion(const QString &s);
    BVersion(const BVersion &other);
    ~BVersion();
protected:
    explicit BVersion(BVersionPrivate &d);
public:
    void setVersion(qint8 major, qint8 minor = -1, qint8 patch = -1, Status s = NoStatus, qint8 extra = -1);
    void setVersion(const QString &s, QChar versionSeparator = QChar('.'), QChar statusSeparator = QChar('-'));
    qint8 major() const;
    qint8 minor() const;
    qint8 patch() const;
    Status status() const;
    qint8 extra() const;
    QString toString(StatusRepresentation r = ShortLowercase, QChar versionSeparator = QChar('.'),
                     QChar statusSeparator = QChar('-')) const;
    int compare(const BVersion &other) const;
    bool isValid() const;
public:
    BVersion &operator =(const BVersion &other);
    bool operator ==(const BVersion &other) const;
    bool operator !=(const BVersion &other) const;
    bool operator <(const BVersion &other) const;
    bool operator >(const BVersion &other) const;
    bool operator <=(const BVersion &other) const;
    bool operator >=(const BVersion &other) const;
    operator QString() const;
    operator QVariant() const;
public:
    friend QDataStream &operator <<(QDataStream &stream, const BVersion &v);
    friend QDataStream &operator >>(QDataStream &stream, BVersion &v);
    friend QDebug operator <<(QDebug dbg, const BVersion &v);
};

Q_DECLARE_METATYPE(BVersion)

#endif // BVERSION_H
