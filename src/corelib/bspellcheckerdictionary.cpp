#include "bspellcheckerdictionary.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

class BSpellCheckerDictionaryPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q);
    ~BSpellCheckerDictionaryPrivate();
public:
    void init();
public:
    //
private:
    Q_DISABLE_COPY(BSpellCheckerDictionaryPrivate)
};

/*============================================================================
================================ BSpellCheckerDictionaryPrivate ==============
============================================================================*/

/*============================== Public constructors =======================*/

BSpellCheckerDictionaryPrivate::BSpellCheckerDictionaryPrivate(BSpellCheckerDictionary *q) :
    BBasePrivate(q)
{
    //
}

BSpellCheckerDictionaryPrivate::~BSpellCheckerDictionaryPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSpellCheckerDictionaryPrivate::init()
{
    //
}

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

/*============================== Public constructors =======================*/

BSpellCheckerDictionary::BSpellCheckerDictionary() :
    BBase(*new BSpellCheckerDictionaryPrivate(this))
{
    d_func()->init();
}

BSpellCheckerDictionary::~BSpellCheckerDictionary()
{
    //
}

/*============================== Public methods ============================*/

//
