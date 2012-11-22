#ifndef BSEARCHEDITORMODULE_H
#define BSEARCHEDITORMODULE_H

class BSearchEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

/*============================================================================
================================ Search Editor Module
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BSearchEditorModule)
    Q_OBJECT
public:
    enum Action
    {
        FindAction,
        FindNextAction
    };
    //
    explicit BSearchEditorModule(QObject *parent = 0);
    ~BSearchEditorModule();
    //
    QString name() const;
    QAction *action(Action type) const;
    QList<QAction *> actions() const;
public slots:
    void find();
    void findNext();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BSearchEditorModule)
};

#endif // BSEARCHEDITORMODULE_H

