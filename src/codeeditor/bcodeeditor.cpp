#include "bcodeeditor.h"
#include "bcodeeditor_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>

/*============================================================================
================================ Code Editor Private
============================================================================*/

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate(q)
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
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

BCodeEditor::BCodeEditor(BCodeEditorPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

