#ifndef BGLOBAL_H
#define BGLOBAL_H

#include <QtGlobal>

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

#if defined(BEQT_BUILD_WIDGETS_LIB)
#   define B_WIDGETS_EXPORT Q_DECL_EXPORT
#else
#   define B_WIDGETS_EXPORT Q_DECL_IMPORT
#endif

#if defined(BEQT_BUILD_CODEEDITOR_LIB)
#   define B_CODEEDITOR_EXPORT Q_DECL_EXPORT
#else
#   define B_CODEEDITOR_EXPORT Q_DECL_IMPORT
#endif

#if defined(Q_OS_MAC)
#   define B_OS_MAC
#elif defined(Q_OS_UNIX)
#   define B_OS_UNIX
#elif defined(Q_OS_WIN)
#   define B_OS_WIN
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

#define B_DECLARE_OBJECT(Class) \
inline Class##PrivateObject *o_func() \
{ \
    return qobject_cast<Class##PrivateObject *>( qGetPtrHelper(_m_o) ); \
} \
inline const Class##PrivateObject *o_func() const \
{ \
    return qobject_cast<const Class##PrivateObject *>( qGetPtrHelper(_m_o) ); \
} \
friend class Class##PrivateObject;

#define B_DECLARE_PRIVATE_O(Class) \
inline Class##Private *p_func() \
{ \
    return reinterpret_cast<Class##Private *>( qGetPtrHelper(_m_p) ); \
} \
inline const Class##Private *p_func() const \
{ \
    return reinterpret_cast<const Class##Private *>( qGetPtrHelper(_m_p) ); \
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

#define B_O(Class) Class##PrivateObject *const o = o_func()

#define B_P(Class) Class##Private *const p = p_func()

#define B_Q(Class) Class *const q = q_func()

#define B_DS(Class) Class##Private *const ds = ds_func()

#define B_QS(Class) Class *const qs = qs_func()

B_CORE_EXPORT bool bTest(bool condition, const char *where, const char *what);
B_CORE_EXPORT const char *bVersion();

#endif // BGLOBAL_H
