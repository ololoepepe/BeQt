#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;

#include <BeQt>

#include <QObject>

class B_CORE_EXPORT BTranslator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(QObject *parent = 0);
    ~BTranslator();
private:
    Q_DISABLE_COPY(BTranslator)
};

#endif // BTRANSLATOR_H
