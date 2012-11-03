#ifndef BTRANSLATOR_P_H
#define BTRANSLATOR_P_H

class BTranslator;

class QString;
class QTranslator;

#include "bnamespace.h"

#include <QList>

class B_CORE_EXPORT BTranslatorPrivate
{
    B_DECLARE_PUBLIC(BTranslator)
public:
    explicit BTranslatorPrivate(BTranslator *q);
    ~BTranslatorPrivate();
    //
    void clear();
    void install();
    void remove();
    void emitLanguageChange();
    //
    BTranslator *const _m_q;
    QString fileName;
    QList<QTranslator *> translators;
    bool installed;
};

#endif // BTRANSLATOR_P_H
