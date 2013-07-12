#ifndef BBOOKMARKSEDITORMODULE_P_H
#define BBOOKMARKSEDITORMODULE_P_H

class BAbstractCodeEditorDocument;

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
    static void setBookmarks(BAbstractCodeEditorDocument *doc, const QList<QPoint> &list);
    static void setCurrentBookmark(BAbstractCodeEditorDocument *doc, const QPoint &pos);
    static QList<QPoint> bookmarks(BAbstractCodeEditorDocument *doc);
    static QPoint currentBookmark(BAbstractCodeEditorDocument *doc);
public:
    void init();
    void checkBookmarks();
public:
    int maxBookmarks;
    QPointer<QAction> actMakeBookmark;
    QPointer<QAction> actGotoNextBookmark;
public Q_SLOTS:
    void retranslateUi();
private:
    Q_DISABLE_COPY(BBookmarksEditorModulePrivate)
};

#endif // BBOOKMARKSEDITORMODULE_P_H
