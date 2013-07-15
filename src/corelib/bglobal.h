#ifndef BGLOBAL_H
#define BGLOBAL_H

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QObject>
#include <QSignalMapper>
#include <QVariant>
#include <QCoreApplication>
#include <QMetaType>

#include <typeinfo>

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
    return reinterpret_cast<Class##Private *>( qGetPtrHelper(_m_d) ); \
} \
inline const Class##Private *d_func() const \
{ \
    return reinterpret_cast<const Class##Private *>( qGetPtrHelper(_m_d) ); \
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
    return reinterpret_cast<Class##Private *>( qGetPtrHelper(static_cast<Class *>( qGetPtrHelper(_m_self) )->_m_d) ); \
}

#define B_DECLARE_PUBLIC_S(Class) \
static Class *qs_func() \
{ \
    return static_cast<Class *>( qGetPtrHelper(Class::_m_self) ); \
}

#define B_D(Class) Class##Private *const d = d_func()

#define B_Q(Class) Class *const q = q_func()

#define B_DS(Class) Class##Private *const ds = ds_func()

#define B_QS(Class) Class *const qs = qs_func()

#define init_once(type, name, value) \
static type name = value; \
static bool _b_##name##Init = false; \
if (!_b_##name##Init && (_b_##name##Init = true))

B_CORE_EXPORT bool bTest(bool condition, const char *where, const char *what);
B_CORE_EXPORT const char *bVersion();
B_CORE_EXPORT void bRegister();
B_CORE_EXPORT QList<int> bRange(int lb, int ub, int step = 0);
B_CORE_EXPORT QList<int> bRangeD(int lb, int ub, unsigned step = 0);
B_CORE_EXPORT QList<int> bRangeR(int lb, int ub, unsigned step = 0);

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

template <typename T> T* bPointer(const QVariant &v)
{
    return (T *) v.value<void *>();
}

template <typename T> QVariant bVariant(T *ptr)
{
    return QVariant::fromValue((void *) ptr);
}

template<typename T> void bRet(const T &)
{
    //
}

template<typename T> void bRet(T *t, const T &tt)
{
    if (t)
        *t = tt;
}

template<typename T, typename U> void bRet(T *t, const T &tt, U *u, const U &uu)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
}

template<typename T, typename U, typename V> void bRet(T *t, const T &tt, U *u, const U &uu, V *v, const V &vv)
{
    if (t)
        *t = tt;
    if (u)
        *u = uu;
    if (v)
        *v = vv;
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

#endif // BGLOBAL_H
