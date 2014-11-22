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

#ifndef BGLOBAL_H
#define BGLOBAL_H

#include <QCoreApplication>
#include <QList>
#include <QObject>
#include <QSignalMapper>
#include <QString>
#include <QVariant>

#if defined(BEQT_BUILD_CORE_LIB)
#   define B_CORE_EXPORT Q_DECL_EXPORT
#else
#   define B_CORE_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_NETWORK_LIB)
#   define B_NETWORK_EXPORT Q_DECL_EXPORT
#else
#   define B_NETWORK_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_SQL_LIB)
#   define B_SQL_EXPORT Q_DECL_EXPORT
#else
#   define B_SQL_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_WIDGETS_LIB)
#   define B_WIDGETS_EXPORT Q_DECL_EXPORT
#else
#   define B_WIDGETS_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_NETWORKWIDGETS_LIB)
#   define B_NETWORKWIDGETS_EXPORT Q_DECL_EXPORT
#else
#   define B_NETWORKWIDGETS_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_CODEEDITOR_LIB)
#   define B_CODEEDITOR_EXPORT Q_DECL_EXPORT
#else
#   define B_CODEEDITOR_EXPORT Q_DECL_IMPORT
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#define B_DECLARE_TRANSLATE_FUNCTION \
static QString translate(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) \
{ \
    return QCoreApplication::translate(context, sourceText, disambiguation, QCoreApplication::CodecForTr, n); \
}
#else
#define B_DECLARE_TRANSLATE_FUNCTION \
static QString translate(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) \
{ \
    return QCoreApplication::translate(context, sourceText, disambiguation, n); \
}
#endif

#define B_DECLARE_PRIVATE(Class) \
inline Class##Private *d_func() \
{ \
    return reinterpret_cast<Class##Private *>(qGetPtrHelper(_m_d)); \
} \
inline const Class##Private *d_func() const \
{ \
    return reinterpret_cast<const Class##Private *>(qGetPtrHelper(_m_d)); \
} \
friend class Class##Private;

#define B_DECLARE_PUBLIC(Class) \
inline Class *q_func() \
{ \
    return static_cast<Class *>(_m_q); \
} \
inline const Class *q_func() const \
{ \
    return static_cast<const Class *>(_m_q); \
} \
friend class Class;

#define B_DECLARE_PRIVATE_S(Class) \
static inline Class##Private *ds_func() \
{ \
    return reinterpret_cast<Class##Private *>(qGetPtrHelper(static_cast<Class *>(qGetPtrHelper(_m_self))->_m_d)); \
}

#define B_DECLARE_PUBLIC_S(Class) \
static Class *qs_func() \
{ \
    return static_cast<Class *>(qGetPtrHelper(Class::_m_self)); \
}

#define B_D(Class) Class##Private *const d = d_func()

#define B_Q(Class) Class *const q = q_func()

#define B_DS(Class) Class##Private *const ds = ds_func()

#define B_QS(Class) Class *const qs = qs_func()

#define do_once(identifier) \
static bool _b_##identifier##Do = false; \
if (!_b_##identifier##Do && (_b_##identifier##Do = true))

#define init_once(type, name, value) \
static type name = value; \
static bool _b_##name##Init = false; \
if (!_b_##name##Init && (_b_##name##Init = true))

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#   define BEQT_ARCH_ALPHA
#   if defined(__alpha_ev4__)
#       define BEQT_ARCH_ALPHA_EV4
#   elif defined(__alpha_ev5__)
#       define BEQT_ARCH_ALPHA_EV5
#   elif defined(__alpha_ev6__)
#       define BEQT_ARCH_ALPHA_EV6
#   endif
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) \
    || defined(_M_AMD64)
#   define BEQT_ARCH_AMD64
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) \
    || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT)
#   define BEQT_ARCH_ARM
#   if defined(__ARM_ARCH_2__)
#       define BEQT_ARCH_ARM_2
#   elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#       define BEQT_ARCH_ARM_3
#   elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#       define BEQT_ARCH_ARM_4
#   elif defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5E__) || (defined(_M_ARM) && (_M_ARM == 5))
#       define BEQT_ARCH_ARM_5
#   elif defined(__ARM_ARCH_5T__) || defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__)
#       define BEQT_ARCH_ARM_5T
#   elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__) || (defined(_M_ARM) && (_M_ARM == 6))
#       define BEQT_ARCH_ARM_6
#   elif defined(__ARM_ARCH_6T2__)
#       define BEQT_ARCH_ARM_6T2
#   elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7S__) || (defined(_M_ARM) && (_M_ARM == 7))
#       define BEQT_ARCH_ARM_7
#   endif
#elif defined(__aarch64__)
#   define BEQT_ARCH_ARM64
#elif defined(__bfin) || defined(__BFIN__)
#   define BEQT_ARCH_BLACKFIN
#elif defined(__convex__)
#   define BEQT_ARCH_CONVEX
#   if defined(__convex_c1__)
#       define BEQT_ARCH_CONVEX_C1
#   elif defined(__convex_c2__)
#       define BEQT_ARCH_CONVEX_C2
#   elif defined(__convex_c32__)
#       define BEQT_ARCH_CONVEX_C32XX_SERIES
#   elif defined(__convex_c34__)
#       define BEQT_ARCH_CONVEX_C34XX_SERIES
#   elif defined(__convex_c38__)
#       define BEQT_ARCH_CONVEX_C38XX_SERIES
#   endif
#elif defined(__epiphany__)
#   define BEQT_ARCH_EPIPHANY
#elif defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
#   define BEQT_ARCH_HP_PA_RISC
#   if defined(_PA_RISC1_0)
#       define BEQT_ARCH_HP_PA_RISC_PA_RISC_1_0
#   elif defined(_PA_RISC1_1) || defined(__HPPA11__) || defined(__PA7100__)
#       define BEQT_ARCH_HP_PA_RISC_PA_RISC_1_1
#   elif defined(_PA_RISC2_0) || defined(__RISC2_0__) || defined(__HPPA20__) || defined(__PA8000__)
#       define BEQT_ARCH_HP_PA_RISC_PA_RISC_2_0
#   endif
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__IA32__) || defined(_M_I86) \
    || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) \
    || defined(__INTEL__)
#   define BEQT_ARCH_INTEL_X86
#   if (defined(_M_IX86) && (_M_IX86 == 300)) || (defined(__I86__) && (__I86__ == 3)) || defined(__i386__)
#       define BEQT_ARCH_INTEL_X86_80386
#   elif (defined(_M_IX86) && (_M_IX86 == 400)) || (defined(__I86__) && (__I86__ == 4)) || defined(__i486__)
#       define BEQT_ARCH_INTEL_X86_80486
#   elif (defined(_M_IX86) && (_M_IX86 == 500)) || (defined(__I86__) && (__I86__ == 5)) || defined(__i586__)
#       define BEQT_ARCH_INTEL_X86_PENTIUM
#   elif (defined(_M_IX86) && (_M_IX86 == 600)) || (defined(__I86__) && (__I86__ == 6)) || defined(__i686__)
#       define BEQT_ARCH_INTEL_X86_PENTIUM_PRO_II
#   endif
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) \
    || defined(__itanium__)
#   define BEQT_ARCH_INTEL_ITANIUM
#elif defined(__m68k__) || defined(M68000) || defined(__MC68K__)
#   define BEQT_ARCH_MOTOROLA_68K
#   if defined(_mc68000__) || defined(__MC68000__)
#       define BEQT_ARCH_MOTOROLA_68K_68000
#   elif defined(__mc68010__)
#       define BEQT_ARCH_MOTOROLA_68K_68010
#   elif defined(__mc68020__) || defined(__MC68020__)
#       define BEQT_ARCH_MOTOROLA_68K_68020
#   elif defined(__mc68030__) || defined(__MC68030__)
#       define BEQT_ARCH_MOTOROLA_68K_68030
#   elif defined(__mc68040__)
#       define BEQT_ARCH_MOTOROLA_68K_68040
#   elif defined(___mc68060__)
#       define BEQT_ARCH_MOTOROLA_68K68060
#   endif
#elif defined(__mips__) || defined(mips) || defined(__mips) || defined(__MIPS__)
#   define BEQT_ARCH_MIPS
#   if defined(_MIPS_ISA_MIPS1) || (defined(__mips) && (__mips == 1))
#       define BEQT_ARCH_MIPS_R2000
#   elif defined(_MIPS_ISA_MIPS1) || defined(_R3000) || (defined(__mips) && (__mips == 1))
#       define BEQT_ARCH_MIPS_R3000
#   elif defined(_MIPS_ISA_MIPS2) || (defined(__mips) && (__mips == 2)) || defined(__MIPS_ISA2__)
#       define BEQT_ARCH_MIPS_R6000
#   elif defined(_R4000)
#       define BEQT_ARCH_MIPS_R4000
#   elif defined(MIPS_ISA_MIPS3) || (defined(__mips) && (__mips == 3)) || defined(__MIPS_ISA3__)
#       define BEQT_ARCH_MIPS_R4400
#   elif defined(_MIPS_ISA_MIPS4) || (defined(__mips) && (__mips == 4)) || defined(__MIPS_ISA4__)
#       define BEQT_ARCH_MIPS_R8000
#   elif defined(_MIPS_ISA_MIPS4) || (defined(__mips) && (__mips == 4)) || defined(__MIPS_ISA4__)
#       define BEQT_ARCH_MIPS_R10000
#   endif
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) \
    || defined(__ppc__) || defined(__ppc64__) || defined(_M_PPC) || defined(_ARCH_PPC)
#   define BEQT_ARCH_POWERPC
#   if defined(_ARCH_440)
#       define BEQT_ARCH_POWERPC_POWERPC_440
#   elif defined(_ARCH_450)
#       define BEQT_ARCH_POWERPC_POWERPC_450
#   elif (defined(_M_PPC) && (_M_PPC == 601)) || defined(__ppc601__) || defined(_ARCH_601)
#       define BEQT_ARCH_POWERPC_POWERPC_601
#   elif (defined(_M_PPC) && (_M_PPC == 603)) || defined(__ppc603__) || defined(_ARCH_603)
#       define BEQT_ARCH_POWERPC_POWERPC_603
#   elif (defined(_M_PPC) && (_M_PPC == 604)) || defined(__ppc604__) || defined(_ARCH_604)
#       define BEQT_ARCH_POWERPC_POWERPC_604
#   elif (defined(_M_PPC) && (_M_PPC == 620))
#       define BEQT_ARCH_POWERPC_POWERPC_620
#   endif
#elif defined(pyr)
#   define BEQT_ARCH_PYRAMID_9810
#elif defined(__THW_RS6000) || defined(_IBMR2) || defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) \
    || defined(_ARCH_PWR3) || defined(_ARCH_PWR4)
#   define BEQT_ARCH_RS_6000
#elif defined(__sparc__) || defined(__sparc)
#   define BEQT_ARCH_SPARC
#   if defined(__sparcv8)
#       define BEQT_ARCH_SPARC_SPARC_V8_SUPERSPARC
#   elif defined(__sparcv9)
#       define BEQT_ARCH_SPARC_SPARC_V9_ULTRASPARC
#   endif
#elif defined(__sh__)
#   define BEQT_ARCH_SUPERH
#elif defined(__370__) || defined(__THW_370__) || defined(__s390__) || defined(__s390x__) || defined(__zarch__) \
    || defined(__SYSC_ZARCH__)
#   define BEQT_ARCH_SYSTEMZ
#elif defined(_TMS320C2XX) || defined(__TMS320C2000__) || defined(_TMS320C5X) || defined(__TMS320C55X__) \
    || defined(_TMS320C6X) || defined(__TMS320C6X__)
#   define BEQT_ARCH_TMS320
#elif defined(__TMS470__)
#   define BEQT_ARCH_TMS470
#endif

template <typename T> T* bPointer(const QVariant &v)
{
    return (T *) v.value<void *>();
}

template<typename T> void bRemoveDuplicates(QList<T> &list)
{
    for (int i = 0; i < list.size(); ++i)
    {
        for (int j = list.size() - 1; j > i; --j)
        {
            if (list.at(i) == list.at(j))
                list.removeAt(j);
        }
    }
}

template<typename T> void bRemoveDuplicates(QList<T> &list, bool (*areEqual)(const T &, const T &))
{
    if (!areEqual)
        return;
    for (int i = 0; i < list.size(); ++i)
    {
        for (int j = list.size() - 1; j > i; --j)
        {
            if (areEqual(list.at(i), list.at(j)))
                list.removeAt(j);
        }
    }
}

template<typename T> void bRet(const T &)
{
    //Calling "return bRet(...)" is the same as calling "return (void) ..."
}

template<typename T> T bRet(T *t, const T &tt)
{
    if (t)
        *t = tt;
    return tt;
}

template<typename T, typename U> T bRet(T *t, const T &tt, U *u, const U &uu)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    return tt;
}

template<typename T, typename U, typename V> T bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
    return tt;
}

template<typename T, typename U, typename V, typename W> T bRet(T *t, const T &tt, U *u, const U &uu, V *v,
                                                                const V &vv, W *w, const W &ww)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
    if (w)
        *w = ww;
    return tt;
}

template<typename T, typename U> U bRet(T *t, const T &tt, const U &uu)
{
    if (t)
        *t = tt;
    return uu;
}

template<typename T, typename U, typename V> V bRet(T *t, const T &tt, U *u, const U &uu, const V &vv)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    return vv;
}

template<typename T, typename U, typename V, typename W> W bRet(T *t, const T &tt, U *u, const U &uu,
                                                                V *v, const V &vv, const W &ww)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
    return ww;
}

template<typename T, typename U, typename V, typename W, typename X> X bRet(T *t, const T &tt, U *u, const U &uu,
                                                                            V *v, const V &vv, W *w, const W &ww,
                                                                            const X &xx)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
    if (w)
        *w = ww;
    return xx;
}

template <typename T> void bReverse(QList<T> &list)
{
    const int sz = list.size();
    for (int i = 0; i < (list.size() / 2); ++i)
        list.swap(i, sz - i - 1);
}

template <typename T> QList<T> bReversed(const QList<T> &list)
{
    QList<T> nlist;
    for (int i = list.size() - 1; i >= 0; --i)
        nlist << list.at(i);
    return nlist;
}

template<typename T> void bSet(T *t, const T &tt)
{
    if (t)
        *t = tt;
}

template<typename T, typename U> void bSet(T *t, const T &tt, U *u, const U &uu)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
}

template<typename T, typename U, typename V> void bSet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
}

template<typename T, typename U, typename V, typename W> void bSet(T *t, const T &tt, U *u, const U &uu,
                                                                   V *v, const V &vv, W *w, const W &ww)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
    if (w)
        *w = ww;
}

template<typename T> bool bSetMapping(QSignalMapper *mapper, QObject *sender, const char *signal, const T &t)
{
    if (!mapper || !sender || !signal)
        return false;
    mapper->setMapping(sender, t);
    QObject::connect(sender, signal, mapper, SLOT(map()));
    return true;
}

template<typename T> bool bSetMappingSender(QSignalMapper *mapper, QObject *sender, const char *signal, const T &t)
{
    if (!mapper || !sender || !signal)
        return false;
    mapper->setMapping(sender, t);
    QObject::connect(sender, signal, mapper, SLOT(map(QObject *)));
    return true;
}

template <typename T> QVariant bVariant(T *ptr)
{
    return QVariant::fromValue((void *) ptr);
}

template<typename T> QList<T> bWithoutDuplicates(const QList<T> &list)
{
    QList<T> nlist = list;
    bRemoveDuplicates(nlist);
    return nlist;
}

template<typename T> QList<T> bWithoutDuplicates(const QList<T> &list, bool (*areEqual)(const T &, const T &))
{
    if (!areEqual)
        return list;
    QList<T> nlist = list;
    bRemoveDuplicates(nlist, areEqual);
    return nlist;
}

B_CORE_EXPORT QList<int> bRange(int lb, int ub, int step = 0);
B_CORE_EXPORT QList<int> bRangeD(int lb, int ub, unsigned step = 0);
B_CORE_EXPORT QList<int> bRangeM(int lb, int ub, unsigned multiplier = 10);
B_CORE_EXPORT QList<int> bRangeR(int lb, int ub, unsigned step = 0);
B_CORE_EXPORT void bRegister();
B_CORE_EXPORT bool bTest(bool condition, const char *where, const char *what);
B_CORE_EXPORT const char *bVersion();

template <typename T> T enum_cast(int i, const QList<T> &enumMembers, T defaultEnumMember)
{
    T t = static_cast<T>(i);
    return (enumMembers.contains(t)) ? t : defaultEnumMember;
}

template <typename T> T enum_cast(int i, T firstEnumMember, T lastEnumMember)
{
    return bRangeD(firstEnumMember, lastEnumMember).contains(i) ? static_cast<T>(i) : firstEnumMember;
}

template <typename T> T enum_cast(const QVariant &v, const QList<T> &enumMembers, T defaultEnumMember)
{
    bool ok = false;
    int i = v.toInt(&ok);
    return ok ? enum_cast(i, enumMembers, defaultEnumMember) : defaultEnumMember;
}

template <typename T> T enum_cast(const QVariant &v, T firstEnumMember, T lastEnumMember)
{
    bool ok = false;
    int i = v.toInt(&ok);
    return (ok && bRangeD(firstEnumMember, lastEnumMember).contains(i)) ? static_cast<T>(i) : firstEnumMember;
}

#endif // BGLOBAL_H
