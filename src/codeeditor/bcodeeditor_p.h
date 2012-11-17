#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorPrivate;

#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

class BCodeEditorPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BCodeEditorPrivateObject(BCodeEditorPrivate *p);
    ~BCodeEditorPrivateObject();
    //
    BCodeEditorPrivate *const _m_p;
public slots:
    //
private:
    Q_DISABLE_COPY(BCodeEditorPrivateObject)
};

class BCodeEditorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditor)
    Q_DECLARE_TR_FUNCTIONS(BCodeEditor)
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
    //
    BCodeEditorPrivateObject *const _m_o;
    //
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
    //
    friend class BCodeEditorPrivateObject;
};

#endif // BCODEEDITOR_P_H

