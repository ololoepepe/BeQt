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

#include "bbookmarkseditormodule.h"
#include "bbookmarkseditormodule_p.h"

#include "babstractcodeeditordocument.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QAction>
#include <QDebug>
#include <QKeySequence>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QPointer>
#include <QVariant>
#include <QVariantList>

/*============================================================================
================================ BBookmarksEditorModulePrivate ===============
============================================================================*/

/*============================== Public constructors =======================*/

BBookmarksEditorModulePrivate::BBookmarksEditorModulePrivate(BBookmarksEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BBookmarksEditorModulePrivate::~BBookmarksEditorModulePrivate()
{
    //
}

/*============================== Static public methods =====================*/

QList<QPoint> BBookmarksEditorModulePrivate::bookmarks(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return QList<QPoint>();
    QList<QPoint> list;
    foreach (const QVariant &v, doc->property("beqt/bookmarks").toList()) {
        if (v.type() == QVariant::Point)
            list << v.toPoint();
    }
    return list;
}

QPoint BBookmarksEditorModulePrivate::currentBookmark(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return BBookmarksEditorModule::InvalidPos;
    QVariant v = doc->property("beqt/currentBookmark");
    return v.type() == QVariant::Point ? v.toPoint() : BBookmarksEditorModule::InvalidPos;
}

void BBookmarksEditorModulePrivate::setBookmarks(BAbstractCodeEditorDocument *doc, const QList<QPoint> &list)
{
    if (!doc)
        return;
    QVariantList vl;
    foreach (const QPoint &pos, list)
        vl << pos;
    doc->setProperty("beqt/bookmarks", vl);
}

void BBookmarksEditorModulePrivate::setCurrentBookmark(BAbstractCodeEditorDocument *doc, const QPoint &pos)
{
    if (!doc)
        return;
    doc->setProperty("beqt/currentBookmark", pos);
}

/*============================== Public methods ============================*/

void BBookmarksEditorModulePrivate::checkBookmarks()
{
    BAbstractCodeEditorDocument *doc = q_func()->currentDocument();
    bool bm = !bookmarks(doc).isEmpty();
    if (!actMakeBookmark.isNull())
        actMakeBookmark->setEnabled(doc);
    if (!actGotoNextBookmark.isNull())
        actGotoNextBookmark->setEnabled(bm);
}

void BBookmarksEditorModulePrivate::init()
{
    B_Q(BBookmarksEditorModule);
    maxBookmarks = 4;
    //
    actMakeBookmark = new QAction(q);
      actMakeBookmark->setIcon(BApplication::icon("bookmark_add"));
      actMakeBookmark->setShortcut(QKeySequence("Ctrl+Shift+K"));
      QObject::connect(actMakeBookmark, SIGNAL(triggered()), q, SLOT(makeBookmark()));
    actGotoNextBookmark = new QAction(q);
      actGotoNextBookmark->setIcon(BApplication::icon("bookmark"));
      actGotoNextBookmark->setShortcut(QKeySequence("Ctrl+K"));
      QObject::connect(actGotoNextBookmark, SIGNAL(triggered()), q, SLOT(gotoNextBookmark()));
    //
    checkBookmarks();
    retranslateUi();
    QObject::connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

/*============================== Public slots ==============================*/

void BBookmarksEditorModulePrivate::retranslateUi()
{
    if (!actMakeBookmark.isNull()) {
        actMakeBookmark->setText(tr("Make bookmark", "act text"));
        actMakeBookmark->setToolTip(tr("Make a bookmark at cursor position", "act toolTip"));
        actMakeBookmark->setWhatsThis(tr("Use this action to make a bookmark at the current cursor position",
                                         "act whatsThis"));
    }
    if (!actGotoNextBookmark.isNull()) {
        actGotoNextBookmark->setText(tr("Next bookmark", "act text"));
        actGotoNextBookmark->setToolTip(tr("Go to next bookmark", "act toolTip"));
        actGotoNextBookmark->setWhatsThis(tr("Use this action to go to next bookmark in current document",
                                             "act whatsThis"));
    }
}

/*============================================================================
================================ BBookmarksEditorModule ======================
============================================================================*/

/*============================== Static public constants ===================*/

const QPoint BBookmarksEditorModule::InvalidPos = QPoint(-1, -1);

/*============================== Public constructors =======================*/

BBookmarksEditorModule::BBookmarksEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BBookmarksEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BBookmarksEditorModule::~BBookmarksEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BBookmarksEditorModule::BBookmarksEditorModule(BBookmarksEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QAction *BBookmarksEditorModule::action(int type)
{
    switch (type) {
    case MakeBookmarkAction:
        return d_func()->actMakeBookmark;
    case GotoNextBookmarkAction:
        return d_func()->actGotoNextBookmark;
    default:
        return 0;
    }
}

QList<QAction *> BBookmarksEditorModule::actions(bool)
{
    const B_D(BBookmarksEditorModule);
    QList<QAction *> list;
    if (!d->actMakeBookmark.isNull())
        list << d->actMakeBookmark;
    if (!d->actGotoNextBookmark.isNull())
        list << d->actGotoNextBookmark;
    return list;
}

QString BBookmarksEditorModule::id() const
{
    return "beqt/bookmarks";
}

int BBookmarksEditorModule::maximumBookmarkCount() const
{
    return d_func()->maxBookmarks;
}

void BBookmarksEditorModule::setMaximumBookmarkCount(int count)
{
    B_D(BBookmarksEditorModule);
    if (count == d->maxBookmarks)
        return;
    if (count < 0)
        count = 0;
    d->maxBookmarks = count;
    if (count > 0) {
        foreach (BAbstractCodeEditorDocument *doc, editor()->documents()) {
            QVariantList vl = doc->property("beqt/bookmarks").toList();
            while (vl.size() > count)
                vl.removeLast();
            doc->setProperty("beqt/bookmarks", vl);
        }

    } else {
        foreach (BAbstractCodeEditorDocument *doc, editor()->documents())
            doc->setProperty( "beqt/bookmarks", QVariant() );
    }
    d->checkBookmarks();
}

/*============================== Public slots ==============================*/

bool BBookmarksEditorModule::gotoBookmark(int index)
{
    if (!currentDocument())
        return false;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks(currentDocument());
    if (index < 0 || index >= list.size())
        return false;
    QPoint pos = list.at(index);
    if (InvalidPos == pos) {
        removeBookmark(index);
        BBookmarksEditorModulePrivate::setCurrentBookmark(currentDocument(), InvalidPos);
        return false;
    }
    currentDocument()->moveCursor(pos);
    d_func()->checkBookmarks();
    return true;
}

bool BBookmarksEditorModule::gotoNextBookmark()
{
    if (!currentDocument())
        return false;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks(currentDocument());
    if (list.isEmpty())
        return false;
    int ind = list.indexOf(BBookmarksEditorModulePrivate::currentBookmark(currentDocument())) + 1;
    return gotoBookmark(list.size() != ind ? ind : 0);
}

void BBookmarksEditorModule::makeBookmark()
{
    if (!currentDocument())
        return;
    B_D(BBookmarksEditorModule);
    QPoint pos = currentDocument()->cursorPositionRowColumn();
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks(currentDocument());
    if (list.contains(pos))
        list.removeAll(pos);
    list.prepend(pos);
    while (list.size() > d->maxBookmarks)
        list.removeLast();
    BBookmarksEditorModulePrivate::setBookmarks(currentDocument(), list);
    d->checkBookmarks();
}

void BBookmarksEditorModule::removeBookmark(int index)
{
    if (!currentDocument())
        return;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks(currentDocument());
    if (index < 0 || index >= list.size())
        return;
    QPoint pos = BBookmarksEditorModulePrivate::currentBookmark(currentDocument());
    int pind = list.indexOf(pos) - 1;
    if (pind < 0)
        pind = 0;
    list.removeAt(index);
    pos = !list.isEmpty() ? list.at(pind) : InvalidPos;
    BBookmarksEditorModulePrivate::setBookmarks(currentDocument(), list);
    BBookmarksEditorModulePrivate::setCurrentBookmark(currentDocument(), pos);
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::removeLastBookmark()
{
    if (!currentDocument())
        return;
    QPoint pos = BBookmarksEditorModulePrivate::currentBookmark(currentDocument());
    if (InvalidPos == pos)
        return;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks(currentDocument());
    removeBookmark(list.indexOf(pos));
}

/*============================== Protected methods =========================*/

void BBookmarksEditorModule::editorSet(BCodeEditor *)
{
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::editorUnset(BCodeEditor *)
{
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *)
{
    d_func()->checkBookmarks();
}
