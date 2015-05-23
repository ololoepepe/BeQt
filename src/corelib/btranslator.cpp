/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "btranslator.h"
#include "btranslator_p.h"

#include "bapplicationbase.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QLocale>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>

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
    q_func()->unload();
}

/*============================== Public methods ============================*/

void BTranslatorPrivate::init()
{
    installed = false;
    loaded = false;
    locale = BApplicationBase::locale();
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

/*!
\class BTranslator
\inmodule BeQtCore
\brief The BTranslator class is used to incapsulate several QTranslators loaded from different locations.

The BTranslator class incapsulates several QTranslators loaded from different locations, which are related to the same
identifier and language (for example, "qt_ru.qm" from the application shared and user resources directories).

The BTranslator serves two purposes: to load translations from common locations implicitly by only specifying their
identifiers, and to make it easier to handle several translation files located in different directories.

\sa {Translations system}
*/

/*!
\property BTranslator::fileName

This property gets and sets the BTranslator's identifier. This is \e not the absolute (or even relative) name of a
file, but a part of the file name (before the locale name and a suffix).

By default, this property is an empty QString.
*/

/*!
\property BTranslator::installed

This property gets the BTranslator's state. This property is true if the translator is registered in BApplicaitonBase
translations system.

By default, this property is false.
*/

/*!
\property BTranslator::loaded

This property gets the BTranslator's state. This property is true if the associated QTranslators are loaded.

By default, this property is false.
*/

/*!
\property BTranslator::locale

This property gets and sets the BTranslator's locale.

By default, this property is BApplicationBase::locale().
*/

/*!
\property BTranslator::valid

This property gets the BTranslator's validity. The BTranslator is valid if it's fileName is not empty.

By default, this property is false.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a translator and sets it's parent to \a parent.
*/

BTranslator::BTranslator(QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a translator and sets it's file name to \a fileName, and parent to \a parent.
*/

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
    d_func()->init();
    d_func()->fileName = fileName;
}

/*!
Constructs a translator. Sets it's locale to \a locale, file name to \a fileName, and parent to \a parent.
*/

BTranslator::BTranslator(const QLocale &locale, const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
    d_func()->init();
    d_func()->fileName = fileName;
    d_func()->locale = locale;
}

/*!
Destroys the object, deleting the associated data object.

The associated QTranslators are also deleted.
*/

BTranslator::~BTranslator()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Parent is set to \a parent.
*/

BTranslator::BTranslator(BTranslatorPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

/*!
Returns the list of locales available for this translator's identifier.

A locale is considered available if the corresponding file exists in one of common locations (builtin, user, or shared
resources directory). The name of a file is "identifier_locale.qm", where identifier is the translator's fileName, and
locale is the locale name (e.g. "ru_RU").
*/

QList<QLocale> BTranslator::availableLocales() const
{
    if (!isValid())
        return QList<QLocale>();
    const B_D(BTranslator);
    QList<QLocale> list;
    QStringList validFiles;
    int fnLen = d->fileName.length();
    QStringList paths = BApplicationBase::locations(BApplicationBase::TranslationsPath);
    foreach (const QString &path, paths) {
        QDir dir(path);
        QStringList files = dir.entryList(QStringList() << "*.qm", QDir::Files);
        foreach (const QString &file, files) {
            if (file.left(fnLen) == d->fileName && !validFiles.contains(file))
                validFiles << file;
        }
    }
    foreach (const QString &validFile, validFiles) {
        int lx = d->fileName.length() + 1;
        QString lName = validFile.mid(lx, validFile.length() - lx - 3);
        QLocale l(lName);
        if (QLocale::c() != l)
            list << l;
    }
    return list;
}

QString BTranslator::fileName() const
{
    return d_func()->fileName;
}

/*!
Registers the translator in the BApplicationBase translations system.

\sa BApplicationBase::installBeqtTranslator()
*/

void BTranslator::install()
{
    if (d_func()->installed)
        return;
    d_func()->install();
    BApplicationBase::installBeqtTranslator(this);
}

bool BTranslator::isInstalled() const
{
    return d_func()->installed;
}

bool BTranslator::isLoaded() const
{
    return d_func()->loaded;
}

bool BTranslator::isValid() const
{
    return !fileName().isEmpty();
}

/*!
Loads this translator. The underlying QTranslators are installed on the QCoreApplication instance.

If \a fileName is empty, this translator's identifier fileName is used. Otherwies, the translator's identifier is set
to \a fileName.

Returns true if the operation is successful; otherwise returns false.
*/

bool BTranslator::load(const QString &fileName)
{
    B_D(BTranslator);
    if (d->loaded)
        return true;
    if (!fileName.isEmpty())
        d->fileName = fileName;
    if (!isValid())
        return false;
    foreach (const QString &dir, BApplicationBase::locations(BApplicationBase::TranslationsPath)) {
        QTranslator *t = new QTranslator;
        if (t->load(d->locale, d->fileName, "_", dir, ".qm"))
            d->translators << t;
        else
            delete t;
    }
    d->loaded = true;
    return true;
}

/*!
\overload
Loads this translator. The underlying QTranslators are installed on the QCoreApplication instance.

The translator's locale is set to \a locale.

If \a fileName is empty, this translator's identifier fileName is used. Otherwies, the translator's identifier is set
to \a fileName.

Returns true if the operation is successful; otherwise returns false.
*/

bool BTranslator::load(const QLocale &locale, const QString &fileName)
{
    if (isLoaded())
        return true;
    d_func()->locale = locale;
    return load(fileName);
}

QLocale BTranslator::locale() const
{
    return d_func()->locale;
}

/*!
Removes the translator from the BApplicationBase translations system.

\sa BApplicationBase::removeBeqtTranslator()
*/

void BTranslator::remove()
{
    if (!d_func()->installed)
        return;
    d_func()->remove();
    BApplicationBase::removeBeqtTranslator(this);
}

void BTranslator::setFileName(const QString &fileName)
{
    B_D(BTranslator);
    bool wasInstalled = d->installed;
    if (wasInstalled)
        d->remove();
    bool wasLoaded = d->loaded;
    if (wasLoaded)
        unload();
    d->fileName = fileName;
    if (!isValid())
        return;
    if (wasLoaded)
        load();
    if (wasInstalled)
        d->install();
}

void BTranslator::setLocale(const QLocale &locale)
{
    B_D(BTranslator);
    bool wasInstalled = d->installed;
    if (wasInstalled)
        d->remove();
    bool wasLoaded = d->loaded;
    if (wasLoaded)
        unload();
    d->locale = locale;
    if (wasLoaded)
        load();
    if (wasInstalled)
      d->install();
}

/*!
Returns the translation for the key (\a context, \a sourceText, \a disambiguation). If none is found, also tries
(\a context, \a sourceText, ""). If that still fails, returns an empty string.

If \a n is not -1, it is used to choose an appropriate form for the translation (e.g. "%n file found" vs. "%n files
found").
*/

QString BTranslator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    foreach (QTranslator *t, d_func()->translators) {
        QString s = t->translate(context, sourceText, disambiguation, n);
        if (!s.isEmpty())
            return s;
    }
    return sourceText;
}

/*!
Returns the list of pointers to the underlying QTranslators.
*/

QList<QTranslator *> BTranslator::translators() const
{
    return d_func()->translators;
}

/*!
Unloads this translator. The underlying QTranslators are removed from the QCoreApplication instance.
*/

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
