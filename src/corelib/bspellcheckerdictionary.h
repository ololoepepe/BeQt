#ifndef BSPELLCHECKERDICTIONARY_H
#define BSPELLCHECKERDICTIONARY_H

class BSpellCheckerDictionaryPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QString>

/*============================================================================
================================ BSpellCheckerDictionary =====================
============================================================================*/

class B_CORE_EXPORT BSpellCheckerDictionary : public BBase
{
    B_DECLARE_PRIVATE(BSpellCheckerDictionary)
public:
    explicit BSpellCheckerDictionary();
    ~BSpellCheckerDictionary();
public:
    //
private:
    Q_DISABLE_COPY(BSpellCheckerDictionary)
};

#endif // BSPELLCHECKERDICTIONARY_H
