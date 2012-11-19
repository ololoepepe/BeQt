#include "bcodeeditor.h"
#include "bcodeeditor_p.h"
#include "bcodeeditordocument.h"
#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QList>
#include <QMap>
#include <QString>

/*============================================================================
================================ Code Editor Private
============================================================================*/

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate(q)
{
    document = 0;
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      twgt = new QTabWidget(q);
        twgt->setMovable(true);
        twgt->setTabsClosable(true);
        connect( twgt, SIGNAL( currentChanged(int) ), this, SLOT( twgtCurrentChanged(int) ) );
        connect( twgt, SIGNAL( tabCloseRequested(int) ), this, SLOT( twgtTabCloseRequested(int) ) );
      vlt->addWidget(twgt);
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    //
}

//

void BCodeEditorPrivate::twgtCurrentChanged(int index)
{
    //
}

void BCodeEditorPrivate::twgtTabCloseRequested(int index)
{
    //
}

/*============================================================================
================================ Code Editor
============================================================================*/

BCodeEditor::BCodeEditor(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    //
}

BCodeEditor::~BCodeEditor()
{
    //
}

//

void BCodeEditor::addModule(BAbstractEditorModule *module)
{
    //
}

void BCodeEditor::setModules(const QList<BAbstractEditorModule *> &list)
{
    //
}

BAbstractEditorModule *BCodeEditor::module(const QString &name) const
{
    return d_func()->modules.value(name);
}

BAbstractEditorModule *BCodeEditor::moduleForFeature(EditorFeature feature) const
{
    return d_func()->essentialModules.value(feature);
}

QList<BAbstractEditorModule *> BCodeEditor::modules() const
{
    return d_func()->modules.values();
}

//

BCodeEditor::BCodeEditor(BCodeEditorPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

