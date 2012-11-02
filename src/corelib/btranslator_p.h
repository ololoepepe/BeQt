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
    explicit BTranslatorPrivate(BTranslator *q, const QString &fileName);
    ~BTranslatorPrivate();
    //
    void clear();
    void install();
    void remove();
    //
    const QString FileName;
    //
    BTranslator *_m_q;
    QList<QTranslator *> translators;
    bool installed;
};

#endif // BTRANSLATOR_P_H
