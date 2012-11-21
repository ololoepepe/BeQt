#ifndef BPLAINTEXTEDIT_P_H
#define BPLAINTEXTEDIT_P_H

class BPlainTextEditPrivate;

#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

/*============================================================================
================================ Plain Text Edit Private
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPlainTextEdit)
    Q_OBJECT
public:
    explicit BPlainTextEditPrivate(BPlainTextEdit *q);
    ~BPlainTextEditPrivate();
    //
    void init();
    //
    bool drag;
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
};

#endif // BPLAINTEXTEDIT_P_H

