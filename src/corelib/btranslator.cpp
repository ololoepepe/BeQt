#include "btranslator.h"
#include "btranslator_p.h"
#include "bcoreapplication.h"
#include "bcoreapplication_p.h"
#include "bnamespace.h"

#include <QObject>
#include <QString>
#include <QLocale>
#include <QStringList>
#include <QTranslator>
#include <QList>
#include <QCoreApplication>
#include <QDir>

#include <QDebug>

/*============================================================================
================================ BTranslatorPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslatorPrivate::BTranslatorPrivate(BTranslator *q) :
    BBasePrivate(q)
{
    //
}

BTranslatorPrivate::~BTranslatorPrivate()
{
    remove();
}

/*============================== Public methods ============================*/

void BTranslatorPrivate::init()
{
    installed = false;
    loaded = false;
    locale = BCoreApplication::locale();
}

bool BTranslatorPrivate::install()
{
    if (installed)
        return true;
    if (!loaded && !q_func()->load())
        return false;
    foreach (QTranslator *t, translators)
        QCoreApplication::installTranslator(t);
    installed = true;
    return true;
}

void BTranslatorPrivate::remove()
{
    if (!installed)
        return;
    foreach (QTranslator *t, translators)
        QCoreApplication::removeTranslator(t);
    installed = false;
}

/*============================================================================
================================ BTranslator =================================
============================================================================*/

/*============================== Public constructors =======================*/

BTranslator::BTranslator(QObject *parent) :
    QObject(parent), BBase( *new BTranslatorPrivate(this) )
{
    BCoreApplicationPrivate::testCoreInit("BTranslator");
    d_func()->init();
}

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BTranslatorPrivate(this) )
{
    BCoreApplicationPrivate::testCoreInit("BTranslator");
    d_func()->init();
    d_func()->fileName = fileName;
}

BTranslator::BTranslator(const QLocale &locale, const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BTranslatorPrivate(this) )
{
    BCoreApplicationPrivate::testCoreInit("BTranslator");
    d_func()->init();
    d_func()->fileName = fileName;
    d_func()->locale = locale;
}

BTranslator::~BTranslator()
{
    //
}

/*============================== Protected constructors ====================*/

BTranslator::BTranslator(BTranslatorPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BTranslator::setFileName(const QString &fileName)
{
    B_D(BTranslator);
    bool wasInstalled = d->installed;
    if (wasInstalled)
        d->remove();
    d->fileName = fileName;
    if (!isValid())
        return;
    if (wasInstalled)
        d->install();
}

void BTranslator::setLocale(const QLocale &locale)
{
    B_D(BTranslator);
    bool wasInstalled = d->installed;
    if (wasInstalled)
        d->remove();
    d->locale = locale;
    if (wasInstalled)
      d->install();
}

bool BTranslator::load(const QString &fileName)
{
    B_D(BTranslator);
    if (d->loaded)
        return true;
    if (!fileName.isEmpty())
        d->fileName = fileName;
    if (!isValid())
        return false;
    QStringList dirs = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = dirs.size() - 1; i >= 0; --i) //User translators come last, having higher priority
    {
        QTranslator *t = new QTranslator;
        if (t->load(d->locale, d->fileName, "_", dirs.at(i), ".qm"))
            d->translators << t;
        else
            delete t;
    }
    d->loaded = true;
    return true;
}

bool BTranslator::load(const QLocale &locale, const QString &fileName)
{
    if (isLoaded())
        return true;
    d_func()->locale = locale;
    return load(fileName);
}

void BTranslator::unload()
{
    B_D(BTranslator);
    if (!d->loaded)
        return;
    if (d->installed)
        d->remove();
    foreach (QTranslator *t, d->translators)
        delete t;
    d->translators.clear();
    d->loaded = false;
}

bool BTranslator::isValid() const
{
    return !fileName().isEmpty();
}

bool BTranslator::isLoaded() const
{
    return d_func()->loaded;
}

QString BTranslator::fileName() const
{
    return d_func()->fileName;
}

QLocale BTranslator::locale() const
{
    return d_func()->locale;
}

QList<QLocale> BTranslator::availableLocales() const
{
    if ( !isValid() )
        return QList<QLocale>();
    const B_D(BTranslator);
    QList<QLocale> list;
    QStringList validFiles;
    int fnLen = d->fileName.length();
    QStringList paths = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    foreach (const QString &path, paths)
    {
        QDir dir(path);
        QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);
        foreach (const QString &file, files)
        {
            if ( file.left(fnLen) == d->fileName && !validFiles.contains(file) )
                validFiles << file;
        }
    }
    foreach (const QString &validFile, validFiles)
    {
        int lx = d->fileName.length() + 1;
        QString lName = validFile.mid(lx, validFile.length() - lx - 3);
        QLocale l(lName);
        if (QLocale::c() != l)
            list << l;
    }
    return list;
}

QList<QTranslator *> BTranslator::translators() const
{
    return d_func()->translators;
}

QString BTranslator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    foreach (QTranslator *t, d_func()->translators)
    {
        QString s = t->translate(context, sourceText, disambiguation, n);
        if (!s.isEmpty())
            return s;
    }
    return sourceText;
}
