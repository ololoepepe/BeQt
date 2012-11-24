#ifndef BOPENSAVEEDITORMODULE_H
#define BOPENSAVEEDITORMODULE_H

#include "babstracteditormodule.h"

class BOpenSaveEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;
class QWidget;
class QStringList;
class QTextCodec;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ Open Save Editor Module
============================================================================*/

class BOpenSaveEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BOpenSaveEditorModule)
    Q_DISABLE_COPY(BOpenSaveEditorModule)
public:
    explicit BOpenSaveEditorModule(QObject *parent = 0);
    ~BOpenSaveEditorModule();
protected:
    BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    bool canGetOpenFileNames() const;
    bool canGetSaveAsFileName() const;
    bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec);
    bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newFileName, QTextCodec *&codec);
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BCodeEditorDocument *doc);
};

#endif // BOPENSAVEEDITORMODULE_H

