#include "btranslator.h"
#include "btranslator_p.h"
#include "bcoreapplication.h"
#include "bcoreapplication_p.h"

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
================================ Translator Private
============================================================================*/

BTranslatorPrivate::BTranslatorPrivate(BTranslator *q) :
    BBasePrivate(q)
{
    installed = false;
}

BTranslatorPrivate::~BTranslatorPrivate()
{
    remove();
}

//

void BTranslatorPrivate::install()
{
    if (installed)
        return;
    QLocale l = BCoreApplication::locale();
    QStringList dirs = BCoreApplication::locations(BCoreApplication::TranslationsPath);
    for (int i = dirs.size() - 1; i >= 0; --i) //User translators come last, having higher priority
    {
        QTranslator *t = new QTranslator;
        if ( t->load(l,fileName, "_", dirs.at(i), ".qm") )
        {
            translators << t;
            QCoreApplication::installTranslator(t);
        }
        else
        {
            t->deleteLater();
        }
    }
    installed = true;
}

void BTranslatorPrivate::remove()
{
    if (!installed)
        return;
    foreach (QTranslator *t, translators)
    {
        QCoreApplication::removeTranslator(t);
        t->deleteLater();
    }
    translators.clear();
    installed = false;
}

void BTranslatorPrivate::emitLanguageChange()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BTranslator") )
        return;
    BCoreApplication::instance()->d_func()->emitLanguageChange();
}

/*============================================================================
================================ Translator
============================================================================*/

BTranslator::BTranslator(QObject *parent) :
    QObject(parent), BBase( *new BTranslatorPrivate(this) )
{
    //
}

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BTranslatorPrivate(this) )
{
    d_func()->fileName = fileName;
}

BTranslator::~BTranslator()
{
    //
}

//

void BTranslator::setFileName(const QString &fileName, bool languageChange)
{
    B_D(BTranslator);
    bool wasInstalled = isInstalled();
    if (wasInstalled)
        d->remove();
    d->fileName = fileName;
    if ( !isValid() )
        return;
    if (wasInstalled)
        d->install();
    if (languageChange)
        d->emitLanguageChange();
}

bool BTranslator::isValid() const
{
    return !fileName().isEmpty();
}

bool BTranslator::isInstalled() const
{
    return d_func()->installed;
}

QString BTranslator::fileName() const
{
    return d_func()->fileName;
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

//

BTranslator::BTranslator(BTranslatorPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
