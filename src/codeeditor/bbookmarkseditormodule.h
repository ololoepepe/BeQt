#ifndef BBOOKMARKSEDITORMODULE_H
#define BBOOKMARKSEDITORMODULE_H

class BBookmarksEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;
class QPoint;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

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
