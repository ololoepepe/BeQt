#ifndef BABSTRACTEDITORMODULE_H
#define BABSTRACTEDITORMODULE_H

class BAbstractEditorModulePrivate;

class QString;

#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ Abstract Editor Module
============================================================================*/

class BAbstractEditorModule : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BAbstractEditorModule)
    Q_OBJECT
public:
    explicit BAbstractEditorModule(QObject *parent = 0);
    ~BAbstractEditorModule();
    //
    virtual QString name() const = 0;
    virtual BCodeEditor::EditorFeature providesFeature() const;
protected:
    BAbstractEditorModule(BAbstractEditorModulePrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BAbstractEditorModule)
};

#endif // BABSTRACTEDITORMODULE_H

