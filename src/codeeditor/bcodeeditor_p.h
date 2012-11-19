#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorDocument;
class BAbstractEditorModule;

class QVBoxLayout;
class QTabWidget;

#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QString>

/*============================================================================
================================ Code Editor Private
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditor)
    B_DECLARE_TR_FUNCTIONS(BCodeEditor, q)
    Q_OBJECT
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
    //
    QMap<QString, BAbstractEditorModule *> modules;
    QMap<BCodeEditor::EditorFeature, BAbstractEditorModule *> essentialModules;
    BCodeEditorDocument *document;
    //
    QVBoxLayout *vlt;
      QTabWidget *twgt;
public slots:
    void twgtCurrentChanged(int index);
    void twgtTabCloseRequested(int index);
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
};

#endif // BCODEEDITOR_P_H

