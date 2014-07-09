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

#ifndef BSPELLCHECKER_P_H
#define BSPELLCHECKER_P_H

class BSpellCheckerDictionary;

#include "bspellchecker.h"

#include "bbaseobject_p.h"

#include <QList>
#include <QMap>
#include <QObject>
#include <QRegExp>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QRegularExpression>
#endif
#include <QString>

/*============================================================================
================================ BSpellCheckerPrivate ========================
============================================================================*/

class B_CORE_EXPORT BSpellCheckerPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSpellChecker)
public:
    int considerLeft;
    int considerRight;
    QMap<QString, BSpellCheckerDictionary *> dicts;
    QMap<QString, bool> ignored;
    QMap<QString, bool> ignoredImplicitly;
    QList<QRegExp> ignoredRx4;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QList<QRegularExpression> ignoredRx5;
#endif
    QString userDictPath;
public:
    explicit BSpellCheckerPrivate(BSpellChecker *q);
    ~BSpellCheckerPrivate();
public:
    static void flush(const QString &fileName, const QStringList &words);
public:
    void init();
public Q_SLOTS:
    void flush();
private:
    Q_DISABLE_COPY(BSpellCheckerPrivate)
};

#endif //BSPELLCHECKER_P_H
