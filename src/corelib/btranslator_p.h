#ifndef BTRANSLATOR_P_H
#define BTRANSLATOR_P_H

class BTranslator;

class QString;
class QTranslator;

#include "bglobal.h"
#include "bbase_p.h"

#include <QList>
#include <QtGlobal>

/*============================================================================
================================ BTranslatorPrivate ==========================
============================================================================*/

class B_CORE_EXPORT BTranslatorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTranslator)
public:
    explicit BTranslatorPrivate(BTranslator *q);
    ~BTranslatorPrivate();
public:
    void init();
    void clear();
    void install();
    void remove();
    void emitLanguageChange();
public:
    QString fileName;
    QList<QTranslator *> translators;
    bool installed;
private:
    Q_DISABLE_COPY(BTranslatorPrivate)
};

#endif // BTRANSLATOR_P_H
