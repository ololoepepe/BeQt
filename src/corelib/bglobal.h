#ifndef BGLOBAL_H
#define BGLOBAL_H

#include <QtGlobal>

#if defined(BEQT_BUILD_CORE_LIB)
#   define B_CORE_EXPORT Q_DECL_EXPORT
#else
#   define B_CORE_EXPORT Q_DECL_IMPORT
#endif

#if defined(Q_OS_MAC)
#   define B_OS_MAC
#elif defined(Q_OS_UNIX)
#   define B_OS_UNIX
#elif defined(Q_OS_WIN)
#   define B_OS_WIN
#endif

#endif // BGLOBAL_H
