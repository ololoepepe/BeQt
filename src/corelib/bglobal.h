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

#define B_DECLARE_SELF(Class) \
    static inline Class *s_func() \
    { \
        return reinterpret_cast<Class *>( qGetPtrHelper(_m_self) ); \
    }

#define B_DECLARE_PRIVATE_S(Class) \
    static inline Class##Private *ds_func() \
    { \
        return s_func()->d_func(); \
    }

B_CORE_EXPORT bool bTest(bool condition, const char *where, const char *what);
B_CORE_EXPORT const char *bVersion();

#endif // BGLOBAL_H
