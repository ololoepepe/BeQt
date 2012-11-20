#ifndef BBOOKMARKSEDITORMODULE_P_H
#define BBOOKMARKSEDITORMODULE_P_H

class BCodeEditorDocument;

class QAction;

#include "bbookmarkseditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QList>
#include <QPoint>
#include <QPointer>

/*============================================================================
================================ Bookmarks Editor Module Private
============================================================================*/

class B_CODEEDITOR_EXPORT BBookmarksEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BBookmarksEditorModule)
    B_DECLARE_TR_FUNCTIONS(BBookmarksEditorModule, q)
    Q_OBJECT
public:
    static void setBookmarks(BCodeEditorDocument *doc, const QList<QPoint> &list);
    static void setCurrentBookmark(BCodeEditorDocument *doc, const QPoint &pos);
    static QList<QPoint> bookmarks(BCodeEditorDocument *doc);
    static QPoint currentBookmark(BCodeEditorDocument *doc);
    //
    explicit BBookmarksEditorModulePrivate(BBookmarksEditorModule *q);
    ~BBookmarksEditorModulePrivate();
    //
    void checkBookmarks();
    //
    static const QPoint InvalidPos;
    //
    int maxBookmarks;
    //
    QPointer<QAction> actMakeBookmark;
    QPointer<QAction> actGotoNextBookmark;
public slots:
    void retranslateUi();
private:
    Q_DISABLE_COPY(BBookmarksEditorModulePrivate)
};

#endif // BBOOKMARKSEDITORMODULE_P_H

