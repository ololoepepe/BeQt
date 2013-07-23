#ifndef BTRANSLATION_H
#define BTRANSLATION_H

class BTranslationPrivate;

class QDataStream;
class QDebug;
class QVariant;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QString>
#include <QMetaType>

/*============================================================================
================================ BTranslation ================================
============================================================================*/

class BTranslation : public BBase
{
    B_DECLARE_PRIVATE(BTranslation)
public:
    static BTranslation translate(const char *context, const char *sourceText, const char *disambiguation = 0,
                                  int n = -1);
    static BTranslation tr(const char *sourceText, const char *disambiguation = 0, int n = -1);
    static QString translate(const BTranslation &t);
    static QString tr(const BTranslation &t);
public:
    explicit BTranslation();
    BTranslation(const BTranslation &other);
    ~BTranslation();
protected:
    explicit BTranslation(BTranslationPrivate &d);
public:
    QString context() const;
    QString sourceText() const;
    QString disambiguation() const;
    int n() const;
    QString translate() const;
    QString tr() const;
    bool isValid() const;
public:
    BTranslation &operator =(const BTranslation &other);
    bool operator ==(const BTranslation &other) const;
    bool operator !=(const BTranslation &other) const;
    operator QString() const;
    operator QVariant() const;
public:
    friend QDataStream &operator <<(QDataStream &stream, const BTranslation &t);
    friend QDataStream &operator >>(QDataStream &stream, BTranslation &t);
    friend QDebug operator <<(QDebug dbg, const BTranslation &t);
};

typedef BTranslation BTr;

Q_DECLARE_METATYPE(BTranslation)

#endif // BTRANSLATION_H

