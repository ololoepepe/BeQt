#ifndef BPLAINTEXTEDIT_P_H
#define BPLAINTEXTEDIT_P_H

class BPlainTextEditPrivate;

#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

/*============================================================================
================================ BPlainTextEditPrivate =======================
============================================================================*/

class B_WIDGETS_EXPORT BPlainTextEditPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPlainTextEdit)
public:
    explicit BPlainTextEditPrivate(BPlainTextEdit *q);
    ~BPlainTextEditPrivate();
public:
    void init();
public:
    bool drag;
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
};

#endif // BPLAINTEXTEDIT_P_H
