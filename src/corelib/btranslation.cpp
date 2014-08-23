/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "btranslation.h"

#include "bbase.h"
#include "bbase_p.h"
#include "bnamespace.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BTranslationPrivate =========================
============================================================================*/

class BTranslationPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTranslation)
public:
    QStringList arguments;
    QString context;
    QString disambiguation;
    int n;
    QString sourceText;
public:
    explicit BTranslationPrivate(BTranslation *q);
    ~BTranslationPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BTranslationPrivate)
};

/*============================================================================
================================ BTranslationPrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslationPrivate::BTranslationPrivate(BTranslation *q) :
    BBasePrivate(q)
{
    //
}

BTranslationPrivate::~BTranslationPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTranslationPrivate::init()
{
    n = -1;
}

/*============================================================================
================================ BTranslation ================================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslation::BTranslation() :
    BBase(*new BTranslationPrivate(this))
{
    d_func()->init();
}

BTranslation::BTranslation(const BTranslation &other) :
    BBase(*new BTranslationPrivate(this))
{
    *this = other;
}

BTranslation::~BTranslation()
{
    //
}

/*============================== Protected constructors ====================*/

BTranslation::BTranslation(BTranslationPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

BTranslation BTranslation::tr(const char *sourceText, const char *disambiguation, int n)
{
    return translate("BTranslation", sourceText, disambiguation, n);
}

QString BTranslation::tr(const BTranslation &t)
{
    return t.translate();
}

BTranslation BTranslation::translate(const char *context, const char *sourceText, const char *disambiguation, int n)
{
    if (n < 0)
        n = -1;
    BTranslation t;
    BTranslationPrivate *dd = t.d_func();
    dd->context = context;
    dd->sourceText = sourceText;
    dd->disambiguation = disambiguation;
    dd->n = n;
    return t;
}

QString BTranslation::translate(const BTranslation &t)
{
    return t.translate();
}

/*============================== Public methods ============================*/

QString BTranslation::argument(int index) const
{
    return (index >= 0 && index < d_func()->arguments.size()) ? d_func()->arguments.at(index) : QString();
}

QStringList BTranslation::arguments() const
{
    return d_func()->arguments;
}

QString BTranslation::context() const
{
    return d_func()->context;
}

QString BTranslation::disambiguation() const
{
    return d_func()->disambiguation;
}

bool BTranslation::isValid() const
{
    return !d_func()->context.isEmpty() && !d_func()->sourceText.isEmpty();
}

int BTranslation::n() const
{
    return d_func()->n;
}

void BTranslation::setArgument(const QString &argument)
{
    d_func()->arguments = (QStringList() << argument);
}

void BTranslation::setArguments(const QStringList &arguments)
{
    d_func()->arguments = arguments;
}

QString BTranslation::sourceText() const
{
    return d_func()->sourceText;
}

QString BTranslation::tr() const
{
    return translate();
}

QString BTranslation::translate() const
{
    const B_D(BTranslation);
    QString s = BeQt::translate(d->context.toUtf8().constData(), d->sourceText.toUtf8().constData(),
                                d->disambiguation.toUtf8().constData(), d->n);
    foreach (const QString &a, d->arguments)
        s = s.arg(a);
    return s;
}

/*============================== Public operators ==========================*/

BTranslation &BTranslation::operator =(const BTranslation &other)
{
    B_D(BTranslation);
    const BTranslationPrivate *dd = other.d_func();
    d->arguments = dd->arguments;
    d->context = dd->context;
    d->sourceText = dd->sourceText;
    d->disambiguation = dd->disambiguation;
    d->n = dd->n;
    return *this;
}

bool BTranslation::operator ==(const BTranslation &other) const
{
    const B_D(BTranslation);
    const BTranslationPrivate *dd = other.d_func();
    return d->arguments == dd->arguments && d->context == dd->context && d->sourceText == dd->sourceText
            && d->disambiguation == dd->disambiguation && d->n == dd->n;
}

bool BTranslation::operator !=(const BTranslation &other) const
{
    return !(*this == other);
}

BTranslation::operator QString() const
{
    return translate();
}

BTranslation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BTranslation &t)
{
    const BTranslationPrivate *d = t.d_func();
    QVariantMap m;
    m.insert("arguments", d->arguments);
    m.insert("context", d->context);
    m.insert("source_text", d->sourceText);
    m.insert("disambiguation", d->disambiguation);
    m.insert("n", d->n);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BTranslation &t)
{
    BTranslationPrivate *d = t.d_func();
    QVariantMap m;
    stream >> m;
    d->arguments = m.value("arguments").toStringList();
    d->context = m.value("context").toString();
    d->sourceText = m.value("source_text").toString();
    d->disambiguation = m.value("disambiguation").toString();
    d->n = m.value("n", -1).toInt();
    if (d->n < 0)
        d->n = -1;
    return stream;
}

QDebug operator <<(QDebug dbg, const BTranslation &t)
{
    dbg.nospace() << "BTranslation(" << t.context() << "," << t.sourceText() << "," << t.disambiguation() << ")";
    return dbg.space();
}
