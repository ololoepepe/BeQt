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
================================ BBookmarksEditorModulePrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BBookmarksEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BBookmarksEditorModule)
public:
    explicit BBookmarksEditorModulePrivate(BBookmarksEditorModule *q);
    ~BBookmarksEditorModulePrivate();
public:
    static void setBookmarks(BCodeEditorDocument *doc, const QList<QPoint> &list);
    static void setCurrentBookmark(BCodeEditorDocument *doc, const QPoint &pos);
    static QList<QPoint> bookmarks(BCodeEditorDocument *doc);
    static QPoint currentBookmark(BCodeEditorDocument *doc);
public:
    void init();
    void checkBookmarks();
public:
    int maxBookmarks;
    QPointer<QAction> actMakeBookmark;
    QPointer<QAction> actGotoNextBookmark;
public slots:
    void retranslateUi();
private:
    Q_DISABLE_COPY(BBookmarksEditorModulePrivate)
};

#endif // BBOOKMARKSEDITORMODULE_P_H

