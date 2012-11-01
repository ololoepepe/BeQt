#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;

class QString;
class QLocale;

#include <BeQt>

#include <QObject>

class B_CORE_EXPORT BTranslator : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(const QString &fileName, QObject *parent = 0);
    ~BTranslator();
    //
    void setLocale(const QLocale &l);
    void reload();
    QString fileName() const;
protected:
    BTranslatorPrivate *_m_d;
private:
    Q_DISABLE_COPY(BTranslator)
};

#endif // BTRANSLATOR_H
