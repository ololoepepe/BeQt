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
#include <QtGlobal>
#include <QRegExp>
#include <QList>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QRegularExpression>
#endif

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
    QList<QRegExp> ignoredRx4;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QList<QRegularExpression> ignoredRx5;
#endif
    int considerLeft;
    int considerRight;
private:
    Q_DISABLE_COPY(BSpellCheckerPrivate)
};

#endif //BSPELLCHECKER_P_H
