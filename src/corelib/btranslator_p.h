#ifndef BTRANSLATOR_P_H
#define BTRANSLATOR_P_H

class BTranslator;

class QString;
class QTranslator;

#include "bglobal.h"
#include "bbase_p.h"
#include "btranslator.h"

#include <QList>
#include <QtGlobal>
#include <QLocale>

/*============================================================================
================================ BTranslatorPrivate ==========================
============================================================================*/

class B_CORE_EXPORT BTranslatorPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTranslator)
public:
    explicit BTranslatorPrivate(BTranslator *q);
    ~BTranslatorPrivate();
public:
    void init();
    bool install();
    void remove();
public:
    QString fileName;
    QLocale locale;
    QList<QTranslator *> translators;
    bool loaded;
    bool installed;
private:
    Q_DISABLE_COPY(BTranslatorPrivate)
};

#endif // BTRANSLATOR_P_H
