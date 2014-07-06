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

#ifndef BTRANSLATOR_H
#define BTRANSLATOR_H

class BTranslatorPrivate;

class QLocale;
class QTranslator;

#include "bbase.h"

#include <QList>
#include <QObject>
#include <QString>

/*============================================================================
================================ BTranslator =================================
============================================================================*/

class B_CORE_EXPORT BTranslator : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTranslator)
public:
    explicit BTranslator(QObject *parent = 0);
    explicit BTranslator(const QString &fileName, QObject *parent = 0);
    explicit BTranslator(const QLocale &locale, const QString &fileName, QObject *parent = 0);
    ~BTranslator();
protected:
    explicit BTranslator(BTranslatorPrivate &d, QObject *parent = 0);
public:
    QList<QLocale> availableLocales() const;
    QString fileName() const;
    void install();
    bool isInstalled() const;
    bool isLoaded() const;
    bool isValid() const;
    bool load(const QString &fileName = QString());
    bool load(const QLocale &locale, const QString &fileName = QString());
    QLocale locale() const;
    void remove();
    void setFileName(const QString &fileName);
    void setLocale(const QLocale &locale);
    QString translate(const char *context, const char *sourceText, const char *disambiguation = 0, int n = -1) const;
    QList<QTranslator *> translators() const;
    void unload();
private:
    Q_DISABLE_COPY(BTranslator)
};

#endif // BTRANSLATOR_H
