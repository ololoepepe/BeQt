#ifndef BBOOKMARKSEDITORMODULE_H
#define BBOOKMARKSEDITORMODULE_H

class BBookmarksEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

/*============================================================================
================================ Bookmarks Editor Module
============================================================================*/

class B_CODEEDITOR_EXPORT BBookmarksEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BBookmarksEditorModule)
public:
    enum Action
    {
        MakeBookmarkAction,
        GotoNextBookmarkAction
    };
    //
    explicit BBookmarksEditorModule(BCodeEditor *edr = 0);
    ~BBookmarksEditorModule();
    //
    QString name() const;
    void setMaximumBookmarkCount(int count);
    int maximumBookmarkCount() const;
    QAction *action(Action type) const;
    QList<QAction *> actions() const;
public slots:
    void makeBookmark();
    void removeBookmark(int index);
    void removeLastBookmark();
    bool gotoBookmark(int index);
    bool gotoNextBookmark();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BBookmarksEditorModule)
};

#endif // BBOOKMARKSEDITORMODULE_H

