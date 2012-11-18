#include "bcodeeditor.h"
#include "bcodeeditor_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>

BCodeEditorPrivateObject::BCodeEditorPrivateObject(BCodeEditorPrivate *p) :
    BBasePrivateObject(p)
{
    //
}

BCodeEditorPrivateObject::~BCodeEditorPrivateObject()
{
    //
}

//

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate( *q, * new BCodeEditorPrivateObject(this) )
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    //
}

//

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor &q, BCodeEditorPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

//

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

