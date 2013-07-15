#ifndef BTRANSLATEFUNCTOR_H
#define BTRANSLATEFUNCTOR_H

class BTranslateFunctorPrivate;
class BTranslator;

class QString;

#include "bglobal.h"
#include "bbase.h"

/*============================================================================
================================ BTranslateFunctor ===========================
============================================================================*/

class B_CORE_EXPORT BTranslateFunctor : public BBase
{
    B_DECLARE_PRIVATE(BTranslateFunctor)
public:
    explicit BTranslateFunctor(BTranslator *t = 0);
    BTranslateFunctor(const BTranslateFunctor &other);
    ~BTranslateFunctor();
public:
    void setTranslator(BTranslator *t);
    BTranslator * translator() const;
public:
    BTranslateFunctor &operator =(const BTranslateFunctor &other);
    QString operator ()(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) const;
};

#endif // BTRANSLATEFUNCTOR_H
