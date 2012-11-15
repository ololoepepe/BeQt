#ifndef BPLAINTEXTEDIT_P_H
#define BPLAINTEXTEDIT_P_H

class BPlainTextEditPrivate;

class QTextCursor;

#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

class BPlainTextEditPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BPlainTextEditPrivateObject(BPlainTextEditPrivate *p);
    ~BPlainTextEditPrivateObject();
    //
    BPlainTextEditPrivate *const _m_p;
public slots:
    void cursorPositionChanged(const QTextCursor &cursor);
    void contentsChange(int position, int charsRemoved, int charsAdded);
private:
    Q_DISABLE_COPY(BPlainTextEditPrivateObject)
};

class BPlainTextEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPlainTextEdit)
    Q_DECLARE_TR_FUNCTIONS(BPlainTextEdit)
public:
    explicit BPlainTextEditPrivate(BPlainTextEdit *q);
    ~BPlainTextEditPrivate();
    //
    void cursorPositionChanged(const QTextCursor &cursor);
    void contentsChange(int position, int charsRemoved, int charsAdded);
    //
    BPlainTextEditPrivateObject *const _m_o;
    //
    bool drag;
private:
    Q_DISABLE_COPY(BPlainTextEditPrivate)
    //
    friend class BPlainTextEditPrivateObject;
};

#endif // BPLAINTEXTEDIT_P_H

