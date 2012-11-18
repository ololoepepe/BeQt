#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorPrivate;

#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

/*============================================================================
================================ Code Editor Private Object
============================================================================*/

class BCodeEditorPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BCodeEditor)
    Q_OBJECT
public:
    explicit BCodeEditorPrivateObject(BCodeEditorPrivate *p);
    ~BCodeEditorPrivateObject();
public slots:
    //
private:
    Q_DISABLE_COPY(BCodeEditorPrivateObject)
};

/*============================================================================
================================ Code Editor Private
============================================================================*/

class BCodeEditorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditor)
    B_DECLARE_OBJECT(BCodeEditor)
    Q_DECLARE_TR_FUNCTIONS(BCodeEditor)
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
    //
protected:
    BCodeEditorPrivate(BCodeEditor &q, BCodeEditorPrivateObject &o);
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
};

#endif // BCODEEDITOR_P_H

