/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
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

#ifndef BBOOKMARKSEDITORMODULE_P_H
#define BBOOKMARKSEDITORMODULE_P_H

class BAbstractCodeEditorDocument;

#include "bbookmarkseditormodule.h"

#include "babstracteditormodule_p.h"

#include <BeQtCore/private/bbase_p.h>

#include <QAction>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QPointer>

/*============================================================================
================================ BBookmarksEditorModulePrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BBookmarksEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BBookmarksEditorModule)
public:
    QPointer<QAction> actGotoNextBookmark;
    QPointer<QAction> actMakeBookmark;
    int maxBookmarks;
public:
    explicit BBookmarksEditorModulePrivate(BBookmarksEditorModule *q);
    ~BBookmarksEditorModulePrivate();
public:
    static QList<QPoint> bookmarks(BAbstractCodeEditorDocument *doc);
    static QPoint currentBookmark(BAbstractCodeEditorDocument *doc);
    static void setBookmarks(BAbstractCodeEditorDocument *doc, const QList<QPoint> &list);
    static void setCurrentBookmark(BAbstractCodeEditorDocument *doc, const QPoint &pos);
public:
    void checkBookmarks();
    void init();
public Q_SLOTS:
    void retranslateUi();
private:
    Q_DISABLE_COPY(BBookmarksEditorModulePrivate)
};

#endif // BBOOKMARKSEDITORMODULE_P_H
