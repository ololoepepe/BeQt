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

#include "bglobal.h"

#include "bpassword.h"
#include "bpersoninfo.h"
#include "bpersoninfolist.h"
#include "bproperties.h"
#include "btranslation.h"
#include "buuid.h"
#include "bversion.h"

#include <QDebug>
#include <QList>
#include <QMetaType>
#include <QString>

/*!
\headerfile <BeQtGlobal>
\title Global BeQt Declarations
\ingroup funclists

\brief The <BeQtGlobal> header file includes the fundamental global declarations. It is included by most other BeQt
header files.

The global declarations include \l{functions} and \l{macros}.

\section1 Functions

The <BeQtGlobal> header file contains several convenience functions for setting some values and returning from function
in one line. These functions take a template type(s) as argument(s). You can pass a pointer to a variable which you
wish to be set, and a value to which it must be set. These functions are typically used when you need to set some error
flag and then immediately return from function.

Example:

\snippet src/corelib/bglobal.cpp 0

<BeQtGlobal> also contains functions that casts an integer (or a QVariant) to some type (usually enum). It is possible
to specify all allowed values and a default value, which is used if the value casted is not in the allowed values.

Example:

\snippet src/corelib/bglobal.cpp 1

There are also some other convenience functions.

\section1 Macros

The <BeQtGlobal> header file provides a range of macros (BEQT_ARCH_*) that are defined if the application is compiled
on the specified platform. For example, the BEQT_ARCH_INTEL_X86 macro is defined if the application is compiled on the
platform with the Intel x86 CPU.

The purpose of these macros is to enable programmers to add CPU architecture specific code to their application.

The other group of macros is (B_*_EXPORT) that are used to export or import BeQt classes and functions when building
the BeQt libraries or when linking against them, respectively.

The remaining macros are convenience macros do_once() and init_once(), which are used to perform some action or set
some variable only the first time the function where they are used is called.

Example:

\snippet src/corelib/bglobal.cpp 2
*/

/*!
\macro BEQT_VERSION_STR
\relates <BeQtGlobal>

This macro expands to a string that specifies BeQt's version number (for example, "4.1.2"). This is the version against
which the application is compiled.

\sa bVersion()
*/

/*!
\macro B_DECLARE_TRANSLATE_FUNCTION
\relates <BeQtGlobal>

This macro expands to a declaration of global static \e translate function, which in turn is a wrapper over the
QCoreApplication::translate method.
*/

/*!
\macro B_DECLARE_PRIVATE(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of inline method d_func (const and non-const versions). \a Class is used as a
prefix of the returned pointer type, and the suffix is "Private". It also declares the resulting type as a friend class
of the class whithin which it is used.

The macro is used for implementing D-pointer approach in BBase- or BBaseObject-derived classes. It returns a casted
(reinterpret_cast) pointer to the data object of the class (_m_d).

\sa B_DECLARE_PUBLIC(), B_DECLARE_PRIVATE_S()
*/

/*!
\macro B_DECLARE_PUBLIC(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of inline method q_func (const and non-const versions). \a Class is used as a name
of the returned pointer type. It also declares the type as a friend class of the class whithin which it is used.

The macro is used for implementing D-pointer approach in BBasePrivate- or BBaseObjectPrivate-derived classes. It
returns a casted (static_cast) pointer to the interface object of the class (_m_q).

\sa B_DECLARE_PRIVATE(), B_DECLARE_PUBLIC_S()
*/

/*!
\macro B_DECLARE_PRIVATE_S(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of static inline method ds_func. \a Class is used as a prefix of the returned
pointer type, and the suffix is "Private".

The macro is used for implementing D-pointer approach in BBase- or BBaseObject-derived classes. It returns a casted
(reinterpret_cast) pointer to the data object of the class (_m_d of the static _m_self member). The macro is used in
pseudo-singletones (like QCoreApplication).

\sa B_DECLARE_PUBLIC_S(), B_DECLARE_PRIVATE()
*/

/*!
\macro B_DECLARE_PUBLIC_S(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of static inline method qs_func. \a Class is used as a name of the returned pointer
type.

The macro is used for implementing D-pointer approach in BBasePrivate- or BBaseObjectPrivate-derived classes. It
returns a casted (static_cast) pointer to the interface object of the class (_m_q of the static _m_self member). The
macro is used in pseudo-singletones (like QCoreApplication).

\sa B_DECLARE_PRIVATE_S(), B_DECLARE_PUBLIC()
*/

/*!
\macro B_D(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of a variable "d" which is initialized with a call to d_func. \a Class is used as
a prefix of the type name.

\sa B_DECLARE_PRIVATE()
*/

/*!
\macro B_Q(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of a variable "q" which is initialized with a call to q_func. \a Class is used as
a type name.

\sa B_DECLARE_PUBLIC()
*/

/*!
\macro B_DS(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of a variable "ds" which is initialized with a call to ds_func. \a Class is used as
a prefix of the type name.

\sa B_DECLARE_PRIVATE_S()
*/

/*!
\macro B_QS(Class)
\relates <BeQtGlobal>

This macro expands to a declaration of a variable "qs" which is initialized with a call to qs_func. \a Class is used as
a type name.

\sa B_DECLARE_PUBLIC_S()
*/

/*!
\macro do_once(identifier)
\relates <BeQtGlobal>

This macro expands to a declaration of a static bool variable with a name based on \a identifier, and a condition which
is true only when it is checked first. On every consequent checks the condition will be false.

The macro may be used to perform some initialization on first function call. See \l{Macros} for details.

\sa init_once()
*/

/*!
\macro init_once(type, name, value)
\relates <BeQtGlobal>

This macro expands to a declaration of a variable of type \a type and with name \a name initialized with \a value, and
a static bool variable with a name based on \a name, and a condition which is true only when it is checked first. On
every consequent checks the condition will be false.

The macro may be used to perform some initialization on first function call. See \l{Macros} for details.

\sa do_once()
*/

/*!
\fn T* bPointer(const QVariant &v)
\relates <BeQtGlobal>

Casts \a v to a pointer of type T. The type must be registered using Q_DECLARE_METATYPE (and Q_DECLARE_OPAQUE_POINTER,
if it is forward-declared).

\sa bVariant()
*/

/*!
\fn void bRemoveDuplicates(QList<T> &list)
\relates <BeQtGlobal>

Removes duplicate entries from the \a list.

\sa bWithoutDuplicates()
*/

/*!
\fn void bRemoveDuplicates(QList<T> &list, bool (*areEqual)(const T &, const T &))
\overload
\relates <BeQtGlobal>

Removes duplicate entries from the \a list. \a areEqual function is used to compare the elements.

\sa bWithoutDuplicates()
*/

/*!
\fn void bRet(const T &t)
\relates <BeQtGlobal>

Convenience template function. The same as returning \a t casted to void.

\sa bSet()
*/

/*!
\fn T bRet(T *t, const T &tt)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt and returns \a tt. If \a t is a null pointer, just
returns \a tt.

\sa bSet()
*/

/*!
\fn T bRet(T *t, const T &tt, U *u, const U &uu)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, and \a u to \a uu, and returns \a tt. If \a t is a null
pointer, or \a u is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn T bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, and \a v to \a vv, and returns \a tt.
If either \a t, \a u, or \a v is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn T bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv, W *w, const W &ww)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, \a v to \a vv, and \a w to \a ww, and
returns \a tt. If either \a t, \a u, \a v, or \a w is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn U bRet(T *t, const T &tt, const U &uu)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt and returns \a uu. If \a t is a null pointer, just
returns \a uu.


\sa bSet()
*/

/*!
\fn V bRet(T *t, const T &tt, U *u, const U &uu, const V &vv)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, and \a u to \a uu, and returns \a vv. If \a t is a null
pointer, or \a u is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn W bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv, const W &ww)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, and \a v to \a vv, and returns \a ww.
If either \a t, \a u, or \a v is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn X bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv, W *w, const W &ww, const X &xx)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, \a v to \a vv, and \a w to \a ww, and
returns \a xx. If either \a t, \a u, \a v, or \a w is a null pointer, their values are not set.

\sa bSet()
*/

/*!
\fn void bReverse(QList<T> &list)
\relates <BeQtGlobal>

Reverses the order of elements in \a list.

\sa bReversed()
*/

/*!
\fn QList<T> bReversed(const QList<T> &list)
\relates <BeQtGlobal>

Returns a list which contains the same elements as \a list, but in the reversed order. The \a list itself is not
modified.

\sa bReverse()
*/

/*!
\fn void bSet(T *t, const T &tt)
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt. If \a t is a null pointer, does nothing.

\sa bRet()
*/

/*!
\fn void bSet(T *t, const T &tt, U *u, const U &uu)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, and \a u to \a uu. If \a t is a null pointer, or \a u
is a null pointer, their values are not set.

\sa bRet()
*/

/*!
\fn void bSet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, and \a v to \a vv. If either \a t, \a u,
or \a v is a null pointer, their values are not set.

\sa bRet()
*/

/*!
\fn void bSet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv, W *w, const W &ww)
\overload
\relates <BeQtGlobal>

Convenience template function. Sets the value of \a t to \a tt, \a u to \a uu, \a v to \a vv, and \a w to \a ww. If
either \a t, \a u, \a v, or \a w is a null pointer, their values are not set.

\sa bRet()
*/

/*!
\fn bool bSetMapping(QSignalMapper *mapper, QObject *sender, const char *signal, const T &t)
\relates <BeQtGlobal>

Sets mapping of \a mapper for object \a sender to value \a t. The sender's \a signal is connected to the mapper's
map() slot.

Returns true on success. If any parameter is invaliid (null pointer), returns false.

Example

\snippet src/corelib/bglobal.cpp 3

\sa bSetMappingSender()
*/

/*!
\fn bool bSetMappingSender(QSignalMapper *mapper, QObject *sender, const char *signal, const T &t)
\relates <BeQtGlobal>

Sets mapping of \a mapper for object \a sender to value \a t. The sender's \a signal is connected to the mapper's
map(QObject *) slot.

Returns true on success. If any parameter is invaliid (null pointer), returns false.

\sa bSetMapping()
*/

/*!
\fn QVariant bVariant(T *ptr)
\relates <BeQtGlobal>

Stores a pointer \a ptr of type T in a QVariant. The type must be registered using Q_DECLARE_METATYPE (and
Q_DECLARE_OPAQUE_POINTER, if it is forward-declared).

\sa bPointer()
*/

/*!
\fn QList<T> bWithoutDuplicates(const QList<T> &list)
\relates <BeQtGlobal>

Returns a copy of \a list with duplicate entries being removed from it. The \a list itself is not modified.

\sa bRemoveDuplicates()
*/

/*!
\fn QList<T> bWithoutDuplicates(const QList<T> &list, bool (*areEqual)(const T &, const T &))
\overload
\relates <BeQtGlobal>

Returns a copy of \a list with duplicate entries being removed from it. The \a list itself is not modified. \a areEqual
function is used to compare the elements.

\sa bRemoveDuplicates()
*/

/*!
\fn T enum_cast(int i, const QList<T> &enumMembers, T defaultEnumMember)
\relates <BeQtGlobal>

Casts \a i to type T (usually enum) using static_cast. Returns the resulting value. If \a i is not in \a enumMembers,
\a defaultEnumMember is returned.
*/

/*!
\fn T enum_cast(int i, T firstEnumMember, T lastEnumMember)
\overload
\relates <BeQtGlobal>

Casts \a i to type T (usually enum) using static_cast. Returns the resulting value. If \a i is not within range
[\a firstEnumMember; \a lastEnumMember], \a firstEnumMember is returned.
*/

/*!
\fn T enum_cast(const QVariant &v, const QList<T> &enumMembers, T defaultEnumMember)
\overload
\relates <BeQtGlobal>

Casts \a v to integer, and then to type T (usually enum) using static_cast. Returns the resulting value. If \a v can
not be casted to integer, or if the integer is not in \a enumMembers, \a defaultEnumMember is returned.
*/

/*!
\fn T enum_cast(const QVariant &v, T firstEnumMember, T lastEnumMember)
\overload
\relates <BeQtGlobal>

Casts \a v to integer, and then to type T (usually enum) using static_cast. Returns the resulting value. If \a v can
not be casted to integer, or if the integer is not within range [\a firstEnumMember; \a lastEnumMember],
\a firstEnumMember is returned.
*/

/*!
\relates <BeQtGlobal>

Returns a list of integers. If \a lb equals \a ub, a list containing only one value is returned. If \a step is equal to
zero, it is set to either 1 (if \a lb is less than \a ub) or -1 (if \a lb is greater than \a ub).

If \a lb is less than \a ub and \a step is less than zero, an empty list is returned. If \a lb is greater than \a ub
and \a step is greater than zero, an empty list is returned.

\sa bRangeD(), bRangeM(), bRangeR()
*/

QList<int> bRange(int lb, int ub, int step)
{
    if (lb == ub)
        return QList<int>() << lb;
    if (!step)
        step = (lb < ub) ? 1 : -1;
    if ((lb < ub && step < 0) || (lb > ub && step > 0))
        return QList<int>();
    QList<int> r;
    if (lb < ub) {
        for (int i = lb; i <= ub; i += step)
            r << i;
    } else {
        for (int i = lb; i >= ub; i += step)
            r << i;
    }
    return r;
}

/*!
\relates <BeQtGlobal>

Returns a list of integers in range [\a lb; \a ub]. \a step is used to increment the values. If \a lb is greater than
\a ub, returns an empty list.

\sa bRange(), bRangeM(), bRangeR()
*/

QList<int> bRangeD(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, (step ? step : 1));
}

/*!
\relates <BeQtGlobal>

Returns a list of integers in range [\a lb; \a ub]. \a multiplier is used to increment the values. If \a lb is greater
than zero and \a ub is less than zero (or vice versa), returns an empty list. If \a lb, \a ub, or \a multiplier is
equal to zero, an empty list is returned.

\sa bRange(), bRangeD(), bRangeR()
*/

QList<int> bRangeM(int lb, int ub, unsigned multiplier)
{
    if (!lb || !ub || !multiplier || (lb < 0 && ub > 0) || (lb > 0 && ub < 0))
        return QList<int>();
    if (lb == ub)
        return QList<int>() << lb;
    if (1 == multiplier)
        return QList<int>();
    QList<int> r;
    if (lb < ub) {
        for (int i = lb; i <= ub; i *= multiplier)
            r << i;
    } else {
        for (int i = lb; i >= ub; i /= multiplier)
            r << i;
    }
    return r;
}

/*!
\relates <BeQtGlobal>

Returns a list of integers in range [\a ub; \a lb]. \a step is used to decrement the values. If \a lb is less than
\a ub, returns an empty list.

\sa bRange(), bRangeD(), bRangeM()
*/

QList<int> bRangeR(int lb, int ub, unsigned step)
{
    return bRange(lb, ub, -1 * (step ? step : 1));
}

/*!
\relates <BeQtGlobal>

Registers BeQt types in Qt's meta-object system using qRegisterMetaType and qRegisterMetaTypeStreamOperators.

\note This function is implicitly called when creating BApplicationBase instance.
*/

void bRegister()
{
    do_once(registered) {
        qRegisterMetaType<BPassword>();
        qRegisterMetaTypeStreamOperators<BPassword>();
        qRegisterMetaType<BPersonInfo>();
        qRegisterMetaTypeStreamOperators<BPersonInfo>();
        qRegisterMetaType<BPersonInfoList>();
        qRegisterMetaTypeStreamOperators<BPersonInfoList>();
        qRegisterMetaType<BProperties>();
        qRegisterMetaTypeStreamOperators<BProperties>();
        qRegisterMetaType<BTranslation>();
        qRegisterMetaTypeStreamOperators<BTranslation>();
        qRegisterMetaType<BTr>("BTr");
        qRegisterMetaTypeStreamOperators<BTr>("BTr");
        qRegisterMetaType<BUuid>();
        qRegisterMetaTypeStreamOperators<BUuid>();
        qRegisterMetaType<BVersion>();
        qRegisterMetaTypeStreamOperators<BVersion>();
    }
}

/*!
\relates <BeQtGlobal>

Checks if \a condition is true. If it is false, calls qCritical, passing \a where and \a what parameters to it.

Returns \a condition.
*/

bool bTest(bool condition, const char *where, const char *what)
{
    if (!condition)
        qCritical() << qPrintable(QString(where) + ": " + QString(what));
    return condition;
}

/*!
\relates <BeQtGlobal>

Returns BeQt's version number (for example, "4.1.2"). This is the version against which the application is compiled.

\sa BEQT_VERSION_STR
*/

const char *bVersion()
{
    return BEQT_VERSION_STR;
}
