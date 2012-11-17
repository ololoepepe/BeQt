#include "bcodeeditor.h"
#include "bcodeeditor_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>

BCodeEditorPrivateObject::BCodeEditorPrivateObject(BCodeEditorPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BCodeEditorPrivateObject::~BCodeEditorPrivateObject()
{
    //
}

//

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate(q), _m_o( new BCodeEditorPrivateObject(this) )
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    _m_o->deleteLater();
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

