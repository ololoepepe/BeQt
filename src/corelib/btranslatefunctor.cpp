#include "btranslatefunctor.h"
#include "btranslator.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>

#include <QDebug>

/*============================================================================
================================ BTranslateFunctorPrivate ====================
============================================================================*/

class B_CORE_EXPORT BTranslateFunctorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTranslateFunctor)
public:
    explicit BTranslateFunctorPrivate(BTranslateFunctor *q);
    ~BTranslateFunctorPrivate();
public:
    void init();
public:
    BTranslator *translator;
private:
    Q_DISABLE_COPY(BTranslateFunctorPrivate)
};

/*============================================================================
================================ BTranslateFunctorPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslateFunctorPrivate::BTranslateFunctorPrivate(BTranslateFunctor *q) :
    BBasePrivate(q)
{
    //
}

BTranslateFunctorPrivate::~BTranslateFunctorPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTranslateFunctorPrivate::init()
{
    translator = 0;
}

/*============================================================================
================================ BTranslateFunctor ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslateFunctor::BTranslateFunctor(BTranslator *t) :
    BBase(*new BTranslateFunctorPrivate(this))
{
    d_func()->init();
    d_func()->translator = t;
}

BTranslateFunctor::BTranslateFunctor(const BTranslateFunctor &other) :
   BBase(*new BTranslateFunctorPrivate(this))
{
    d_func()->init();
    *this = other;
}

BTranslateFunctor::~BTranslateFunctor()
{
    //
}

/*============================== Public methods ============================*/

void BTranslateFunctor::setTranslator(BTranslator *t)
{
    d_func()->translator = t;
}

BTranslator *BTranslateFunctor::translator() const
{
    return d_func()->translator;
}

/*============================== Public operators ==========================*/

BTranslateFunctor &BTranslateFunctor::operator =(const BTranslateFunctor &other)
{
    d_func()->translator = other.d_func()->translator;
    return *this;
}

QString BTranslateFunctor::operator ()(const char *context, const char *sourceText, const char *disambiguation,
                                       int n) const
{
    return d_func()->translator ? d_func()->translator->translate(context, sourceText, disambiguation, n) :
                                  QString(sourceText);
}
