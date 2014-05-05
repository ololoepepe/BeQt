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

#ifndef BBOOKMARKSEDITORMODULE_H
#define BBOOKMARKSEDITORMODULE_H

class BBookmarksEditorModulePrivate;
class BCodeEditor;
class BAbstractCodeEditorDocument;

class QString;
class QAction;
#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>
#include <QPoint>

/*============================================================================
================================ BBookmarksEditorModule ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BBookmarksEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BBookmarksEditorModule)
public:
    enum Action
    {
        MakeBookmarkAction,
        GotoNextBookmarkAction
    };
public:
    static const QPoint InvalidPos;
public:
    explicit BBookmarksEditorModule(QObject *parent = 0);
    ~BBookmarksEditorModule();
protected:
    explicit BBookmarksEditorModule(BBookmarksEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    void setMaximumBookmarkCount(int count);
    int maximumBookmarkCount() const;
    QAction *action(int type);
    QList<QAction *> actions(bool extended = false);
public Q_SLOTS:
    void makeBookmark();
    void removeBookmark(int index);
    void removeLastBookmark();
    bool gotoBookmark(int index);
    bool gotoNextBookmark();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BBookmarksEditorModule)
};

#endif // BBOOKMARKSEDITORMODULE_H
