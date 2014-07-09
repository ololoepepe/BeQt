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

/*============================== Public constructors =======================*/

BTranslator::BTranslator(QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
    d_func()->init();
}

BTranslator::BTranslator(const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
    d_func()->init();
    d_func()->fileName = fileName;
}

BTranslator::BTranslator(const QLocale &locale, const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BTranslatorPrivate(this))
{
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

bool BTranslator::load(const QString &fileName)
{
    B_D(BTranslator);
    if (d->loaded)
        return true;
    if (!fileName.isEmpty())
        d->fileName = fileName;
    if (!isValid())
        return false;
    QStringList dirs = BApplicationBase::locations(BApplicationBase::TranslationsPath);
    for (int i = dirs.size() - 1; i >= 0; --i) { //User translators come last, having higher priority
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

QLocale BTranslator::locale() const
{
    return d_func()->locale;
}

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

QString BTranslator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    foreach (QTranslator *t, d_func()->translators) {
        QString s = t->translate(context, sourceText, disambiguation, n);
        if (!s.isEmpty())
            return s;
    }
    return sourceText;
}

QList<QTranslator *> BTranslator::translators() const
{
    return d_func()->translators;
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
