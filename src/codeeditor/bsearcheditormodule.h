#ifndef BSEARCHEDITORMODULE_H
#define BSEARCHEDITORMODULE_H

class BSearchEditorModulePrivate;
class BCodeEditor;
class BAbstractCodeEditorDocument;

class QString;
class QAction;
class QByteArray;

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
    QAction *action(int type);
    QList<QAction *> actions(bool extended = false);
    QByteArray saveState() const;
    void restoreState(const QByteArray &state);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setMatchWholeWords(bool enabled);
    void setBackwardOrder(bool enabled);
    void setCyclicSearch(bool enabled);
    void setMaximumHistorySize(int sz);
    void setReplaceEnabled(bool enabled);
    Qt::CaseSensitivity caseSensitivity() const;
    bool matchWholeWords() const;
    bool backwardOrder() const;
    bool cyclicSearch() const;
    int maximumHistorySize() const;
    bool replaceEnabled() const;
public Q_SLOTS:
    void find();
    void findNext(bool showDialog = true);
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
Q_SIGNALS:
    void textFound(const QString &text);
    void textNotFound(const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
    void message(const QString &msg);
private:
    Q_DISABLE_COPY(BSearchEditorModule)
};

#endif // BSEARCHEDITORMODULE_H
