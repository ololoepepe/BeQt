#include "bbookmarkseditormodule.h"
#include "bbookmarkseditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QAction>
#include <QList>
#include <QPoint>
#include <QVariant>
#include <QVariantList>
#include <QPointer>

#include <QDebug>

/*============================================================================
================================ Bookmarks Editor Module Private
============================================================================*/

void BBookmarksEditorModulePrivate::setBookmarks(BCodeEditorDocument *doc, const QList<QPoint> &list)
{
    if (!doc)
        return;
    QVariantList vl;
    foreach (const QPoint &pos, list)
        vl << pos;
    doc->setProperty("beqt/bookmarks", vl);
}

void BBookmarksEditorModulePrivate::setCurrentBookmark(BCodeEditorDocument *doc, const QPoint &pos)
{
    if (!doc)
        return;
    doc->setProperty("beqt/currentBookmark", pos);
}

QList<QPoint> BBookmarksEditorModulePrivate::bookmarks(BCodeEditorDocument *doc)
{
    if (!doc)
        return QList<QPoint>();
    QList<QPoint> list;
    foreach ( const QVariant &v, doc->property("beqt/bookmarks").toList() )
        if (v.type() == QVariant::Point)
            list << v.toPoint();
    return list;
}

QPoint BBookmarksEditorModulePrivate::currentBookmark(BCodeEditorDocument *doc)
{
    if (!doc)
        return InvalidPos;
    QVariant v = doc->property("beqt/currentBookmark");
    return v.type() == QVariant::Point ? v.toPoint() : InvalidPos;
}

//

BBookmarksEditorModulePrivate::BBookmarksEditorModulePrivate(BBookmarksEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BBookmarksEditorModulePrivate::~BBookmarksEditorModulePrivate()
{
    //
}

//

void BBookmarksEditorModulePrivate::init()
{
    B_Q(BBookmarksEditorModule);
    maxBookmarks = 4;
    //
    actMakeBookmark = new QAction(this);
      actMakeBookmark->setIcon( BApplication::icon("bookmark_add") );
      connect( actMakeBookmark.data(), SIGNAL( triggered() ), q, SLOT( makeBookmark() ) );
    actGotoNextBookmark = new QAction(this);
      actGotoNextBookmark->setIcon( BApplication::icon("bookmark") );
      connect( actGotoNextBookmark.data(), SIGNAL( triggered() ), q, SLOT( gotoNextBookmark() ) );
    //
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BBookmarksEditorModulePrivate::checkBookmarks()
{
    BCodeEditorDocument *doc = q_func()->currentDocument();
    bool bm = !bookmarks(doc).isEmpty();
    if ( !actMakeBookmark.isNull() )
        actMakeBookmark->setEnabled(doc);
    if ( !actGotoNextBookmark.isNull() )
        actGotoNextBookmark->setEnabled(bm);
}

//

void BBookmarksEditorModulePrivate::retranslateUi()
{
    if ( !actMakeBookmark.isNull() )
    {
        actMakeBookmark->setText( trq("make bookmark", "act text") );
        actMakeBookmark->setToolTip( trq("make bookmark", "act toolTip") );
        actMakeBookmark->setWhatsThis( trq("make bookmark", "act whatsThis") );
    }
    if ( !actGotoNextBookmark.isNull() )
    {
        actGotoNextBookmark->setText( trq("go to next bookmark", "act text") );
        actGotoNextBookmark->setToolTip( trq("go to next bookmark", "act toolTip") );
        actGotoNextBookmark->setWhatsThis( trq("go to next bookmark", "act whatsThis") );
    }
}

//

const QPoint BBookmarksEditorModulePrivate::InvalidPos = QPoint(-1, -1);

/*============================================================================
================================ Bookmarks Editor Module
============================================================================*/

BBookmarksEditorModule::BBookmarksEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BBookmarksEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BBookmarksEditorModule::~BBookmarksEditorModule()
{
    //
}

//

QString BBookmarksEditorModule::id() const
{
    return "beqt/bookmarks";
}

void BBookmarksEditorModule::setMaximumBookmarkCount(int count)
{
    B_D(BBookmarksEditorModule);
    if (count == d->maxBookmarks)
        return;
    if (count < 0)
        count = 0;
    d->maxBookmarks = count;
    if (count > 0)
    {
        foreach ( BCodeEditorDocument *doc, editor()->documents() )
        {
            QVariantList vl = doc->property("beqt/bookmarks").toList();
            while (vl.size() > count)
                vl.removeLast();
            doc->setProperty("beqt/bookmarks", vl);
        }

    }
    else
    {
        foreach ( BCodeEditorDocument *doc, editor()->documents() )
            doc->setProperty( "beqt/bookmarks", QVariant() );
    }
    d->checkBookmarks();
}

int BBookmarksEditorModule::maximumBookmarkCount() const
{
    return d_func()->maxBookmarks;
}

QAction *BBookmarksEditorModule::action(Action type) const
{
    switch (type)
    {
    case MakeBookmarkAction:
        return d_func()->actMakeBookmark.data();
    case GotoNextBookmarkAction:
        return d_func()->actGotoNextBookmark.data();
    default:
        return 0;
    }
}

QList<QAction *> BBookmarksEditorModule::actions() const
{
    const B_D(BBookmarksEditorModule);
    QList<QAction *> list;
    if ( !d->actMakeBookmark.isNull() )
        list << d->actMakeBookmark.data();
    if ( !d->actGotoNextBookmark.isNull() )
        list << d->actGotoNextBookmark.data();
    return list;
}

//

void BBookmarksEditorModule::makeBookmark()
{
    if ( !currentDocument() )
        return;
    B_D(BBookmarksEditorModule);
    QPoint pos = currentDocument()->cursorPosition();
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks( currentDocument() );
    if ( list.contains(pos) )
        list.removeAll(pos);
    list.prepend(pos);
    while (list.size() > d->maxBookmarks)
        list.removeLast();
    BBookmarksEditorModulePrivate::setBookmarks(currentDocument(), list);
    d->checkBookmarks();
}

void BBookmarksEditorModule::removeBookmark(int index)
{
    if ( !currentDocument() )
        return;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks( currentDocument() );
    if ( index < 0 || index >= list.size() )
        return;
    QPoint pos = BBookmarksEditorModulePrivate::currentBookmark( currentDocument() );
    int pind = list.indexOf(pos) - 1;
    if (pind < 0)
        pind = 0;
    list.removeAt(index);
    pos = !list.isEmpty() ? list.at(pind) : BBookmarksEditorModulePrivate::InvalidPos;
    BBookmarksEditorModulePrivate::setBookmarks(currentDocument(), list);
    BBookmarksEditorModulePrivate::setCurrentBookmark(currentDocument(), pos);
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::removeLastBookmark()
{
    if ( !currentDocument() )
        return;
    QPoint pos = BBookmarksEditorModulePrivate::currentBookmark( currentDocument() );
    if (BBookmarksEditorModulePrivate::InvalidPos == pos)
        return;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks( currentDocument() );
    removeBookmark( list.indexOf(pos) );
}

bool BBookmarksEditorModule::gotoBookmark(int index)
{
    if ( !currentDocument() )
        return false;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks( currentDocument() );
    if ( index < 0 || index >= list.size() )
        return false;
    QPoint pos = list.at(index);
    if (BBookmarksEditorModulePrivate::InvalidPos == pos)
    {
        removeBookmark(index);
        BBookmarksEditorModulePrivate::setCurrentBookmark(currentDocument(),
                                                          BBookmarksEditorModulePrivate::InvalidPos);
        return false;
    }
    currentDocument()->moveCursor(pos);
    d_func()->checkBookmarks();
    return true;
}

bool BBookmarksEditorModule::gotoNextBookmark()
{
    if ( !currentDocument() )
        return false;
    QList<QPoint> list = BBookmarksEditorModulePrivate::bookmarks( currentDocument() );
    if ( list.isEmpty() )
        return false;
    int ind = list.indexOf( BBookmarksEditorModulePrivate::currentBookmark( currentDocument() ) ) + 1;
    return gotoBookmark(list.size() != ind ? ind : 0);
}

//

void BBookmarksEditorModule::editorSet(BCodeEditor *edr)
{
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::editorUnset(BCodeEditor *edr)
{
    d_func()->checkBookmarks();
}

void BBookmarksEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->checkBookmarks();
}
