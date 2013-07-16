#ifndef BSPELLCHECKER_P_H
#define BSPELLCHECKER_P_H

class BSpellCheckerDictionary;

#include "bspellchecker.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QObject>
#include <QString>
#include <QMap>
#include <QSet>
#include <QVariant>

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

class BSpellCheckerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSpellChecker)
public:
    static void flush(const QString &fileName, const QStringList &words);
public:
    explicit BSpellCheckerPrivate(BSpellChecker *q);
    ~BSpellCheckerPrivate();
public:
    void init();
public Q_SLOTS:
    void flush();
public:
    QMap<QString, BSpellCheckerDictionary *> dicts;
    QString userDictPath;
    QSet<QString> ignored;
    QSet<QString> ignoredImplicitly;
private:
    Q_DISABLE_COPY(BSpellCheckerPrivate)
};

#endif //BSPELLCHECKER_P_H
