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

/*!
\typedef BTr
\relates BTranslation

The BTr typedef provides a synonym for BTranslation.
*/

/*!
\class BTranslation
\inmodule BeQtCore
\brief The BTranslation class provides an easy to store and (de)serialize translation.

Qt does not provide convenient means of storing and (de)serializing translations. The QT_TRANSLATE_NOOP3 macro is not
powerful enough. BTranslation solves this issue.

For an example, see BDynamicTranslator.
*/

/*============================== Public constructors =======================*/

/*!
Constructs an empty translation.
*/

BTranslation::BTranslation() :
    BBase(*new BTranslationPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a copy of \a other.
*/

BTranslation::BTranslation(const BTranslation &other) :
    BBase(*new BTranslationPrivate(this))
{
    *this = other;
}

/*!
Destroys the object, deleting the associated data object.
*/

BTranslation::~BTranslation()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it.
*/

BTranslation::BTranslation(BTranslationPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

/*!
Creates a translation for \a sourceText and (optional) \a disambiguation. Returns that translation.

If \a n is not -1, it is used to choose an appropriate form for the translation (e.g. "%n file found" vs. "%n files
found").

The context is "BTranslation".

\sa translate()
*/

BTranslation BTranslation::tr(const char *sourceText, const char *disambiguation, int n)
{
    return translate("BTranslation", sourceText, disambiguation, n);
}

/*!
\overload
Returns the translated source text of the translation \a t.

\sa translate()
*/

QString BTranslation::tr(const BTranslation &t)
{
    return t.translate();
}

/*!
Creates a translation for \a sourceText and (optional) \a disambiguation in the context \a context. Returns that
translation.

If \a n is not -1, it is used to choose an appropriate form for the translation (e.g. "%n file found" vs. "%n files
found").

\sa tr()
*/

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

/*!
\overload
Returns the translated source text of the translation \a t.

\sa tr()
*/

QString BTranslation::translate(const BTranslation &t)
{
    return t.translate();
}

/*============================== Public methods ============================*/

/*!
Returns the argument at the position \a index of this translation.

\sa arguments(), setArgument(), setArguments()
*/

QString BTranslation::argument(int index) const
{
    return (index >= 0 && index < d_func()->arguments.size()) ? d_func()->arguments.at(index) : QString();
}

/*!
Returns the arguments of this translation.

When translating, the positional arguments contained in the source text ("%1", "%2", and so on) are replaced with the
these arguments successively.

\sa argument(), setArgument(), setArguments()
*/

QStringList BTranslation::arguments() const
{
    return d_func()->arguments;
}

/*!
Returns the context of this translation.
*/

QString BTranslation::context() const
{
    return d_func()->context;
}

/*!
Returns the disambiguation of this translation.
*/

QString BTranslation::disambiguation() const
{
    return d_func()->disambiguation;
}

/*!
Returns true if this translation's context and source text are not empty; otherwise returns false.
*/

bool BTranslation::isValid() const
{
    return !d_func()->context.isEmpty() && !d_func()->sourceText.isEmpty();
}

/*!
Returns the \e n of this translation.
*/

int BTranslation::n() const
{
    return d_func()->n;
}

/*!
Sets the argument of this translation to \a argument.

\sa argument(), arguments(), setArguments()
*/

void BTranslation::setArgument(const QString &argument)
{
    d_func()->arguments = (QStringList() << argument);
}

/*!
Sets the arguments of this translation to \a arguments.

\sa argument(), arguments(), setArgument()
*/

void BTranslation::setArguments(const QStringList &arguments)
{
    d_func()->arguments = arguments;
}

/*!
Returns the source text of this translation.
*/

QString BTranslation::sourceText() const
{
    return d_func()->sourceText;
}

/*!
\overload
Returns the translated source text of this translation.

\sa translate()
*/

QString BTranslation::tr() const
{
    return translate();
}

/*!
\overload
Returns the translated source text of this translation.

\sa tr()
*/

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

/*!
Assigns \a other to this translation and returns a reference to this translation.
*/

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

/*!
Returns true if this translation is equal to translation \a other; otherwise returns false.

The translations are equal if their arguments, contexts, source texts, disambiguations and \e {n}'s are equal.
*/

bool BTranslation::operator ==(const BTranslation &other) const
{
    const B_D(BTranslation);
    const BTranslationPrivate *dd = other.d_func();
    return d->arguments == dd->arguments && d->context == dd->context && d->sourceText == dd->sourceText
            && d->disambiguation == dd->disambiguation && d->n == dd->n;
}

/*!
Returns true if this translation is not equal to translation \a other; otherwise returns false.

The translations are equal if their arguments, contexts, source texts, disambiguations and \e {n}'s are equal.
*/

bool BTranslation::operator !=(const BTranslation &other) const
{
    return !(*this == other);
}

/*!
Returns the translated source text of this translation.

\sa tr(), translate()
*/

BTranslation::operator QString() const
{
    return translate();
}

/*!
Stores the translation in a QVariant. Returns that QVariant.
*/

BTranslation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

/*!
\relates BTranslation
Writes the translation \a t to the data stream \a stream.

Returns a reference to the stream.
*/

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

/*!
\relates BTranslation
Reads a translation from the data stream \a stream into \a t.

Returns a reference to the stream.
*/

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

/*!
\relates BTranslation
Writes the translation \a t to the output stream for debugging information \a dbg and returns a reference to the
stream.
*/

QDebug operator <<(QDebug dbg, const BTranslation &t)
{
    dbg.nospace() << "BTranslation(" << t.context() << "," << t.sourceText() << "," << t.disambiguation() << ")";
    return dbg.space();
}
