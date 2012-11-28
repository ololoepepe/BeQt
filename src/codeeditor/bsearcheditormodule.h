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
================================ BSearchEditorModule =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSearchEditorModule)
public:
    enum Action
    {
        FindAction,
        FindNextAction
    };
public:
    explicit BSearchEditorModule(QObject *parent = 0);
    ~BSearchEditorModule();
protected:
    explicit BSearchEditorModule(BSearchEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(Action type) const;
    QList<QAction *> actions() const;
public slots:
    void find();
    void findNext();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BCodeEditorDocument *doc);
signals:
    void textFound(const QString &text);
    void textNotFound(const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
    void message(const QString &msg);
private:
    Q_DISABLE_COPY(BSearchEditorModule)
};

#endif // BSEARCHEDITORMODULE_H

