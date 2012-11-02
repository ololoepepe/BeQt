#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;
class BCoreApplicationPrivate;

class QString;
class QLocale;

#include <BeQt>

#include <QObject>
#include <QList>

class B_CORE_EXPORT BTranslator : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(QObject *parent = 0);
    ~BTranslator();
    //
    void setFileName(const QString &fileName);
    bool isValid() const;
    QString fileName() const;
    QList<QLocale> availableLocales() const;
protected:
    BTranslatorPrivate *_m_d;
private:
    Q_DISABLE_COPY(BTranslator)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BTRANSLATOR_H
