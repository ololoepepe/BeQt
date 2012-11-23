#ifndef BINDICATORSEDITORMODULE_H
#define BINDICATORSEDITORMODULE_H

class BIndicatorsEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;

class QString;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ Indicators Editor Module
============================================================================*/

class BIndicatorsEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BIndicatorsEditorModule)
    Q_OBJECT
public:
    explicit BIndicatorsEditorModule(QObject *parent = 0);
    ~BIndicatorsEditorModule();
    //
    QString name() const;
protected:
    BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent = 0);
    //
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BIndicatorsEditorModule)
};

#endif // BINDICATORSEDITORMODULE_H

