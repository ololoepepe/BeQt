#ifndef BTRANSLATOR_P_H
#define BTRANSLATOR_P_H

class BTranslator;

class QString;
class QTranslator;

#include <BeQt>

#include <QList>

class BTranslatorPrivate
{
    B_DECLARE_PUBLIC(BTranslator)
public:
    explicit BTranslatorPrivate(BTranslator *q);
    ~BTranslatorPrivate();
    //
    void clear();
    void install();
    void remove();
    //
    BTranslator *_m_q;
    QString fileName;
    QList<QTranslator *> translators;
    bool installed;
};

#endif // BTRANSLATOR_P_H
