#include "btranslator.h"

#include <BCoreApplication>

#include <private/bcoreapplication_p.h>

#include <QObject>
#include <QString>
#include <QLocale>

struct BTranslatorPrivate
{
    QLocale locale;
    //
    BTranslatorPrivate()
    {
        BCoreApplication *app = BCoreApplication::instance();
        locale = app ? app->d_func()->locale : QLocale::system();
    }
    ~BTranslatorPrivate()
    {
        //
    }
};

//

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d(new BTranslatorPrivate)
{
    //
}

BTranslator::~BTranslator()
{
    //
}

//

void BTranslator::setLocale(const QLocale &l)
{
    //
}

void BTranslator::reload()
{
    //
}

QString BTranslator::fileName() const
{
    //
}
