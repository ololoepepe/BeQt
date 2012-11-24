#ifndef BOPENSAVEEDITORMODULE_H
#define BOPENSAVEEDITORMODULE_H

#include "babstracteditormodule.h"

class BOpenSaveEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QAction;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>

/*============================================================================
================================ Open Save Editor Module
============================================================================*/

class BOpenSaveEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BOpenSaveEditorModule)
    Q_DISABLE_COPY(BOpenSaveEditorModule)
public:
    enum Action
    {
        NewFileAction,
        OpenFilesAction,
        ReopenAction,
        SaveFileAction,
        SaveFileAsAction,
        SaveAllFilesAction,
        CloseFileAction,
        CloseAllFilesAction
    };
    //
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(Action type) const;
    QList<QAction *> openActions() const;
    QList<QAction *> saveActions() const;
    QList<QAction *> closeActions() const;
    QList<QAction *> actions() const;
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentReadOnlyChanged(bool ro);
    void documentModificationChanged(bool modified);
    void documentBuisyChanged(bool buisy);
    void currentDocumentChanged(BCodeEditorDocument *doc);
};

#endif // BOPENSAVEEDITORMODULE_H

