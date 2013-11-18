#ifndef BSIMPLECODEEDITORDOCUMENT_P_H
#define BSIMPLECODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BLineNumberWidget;

class QTextDocument;
class QWidget;
class QPlainTextEdit;
class QEvent;
class QKeyEvent;

#include "bsimplecodeeditordocument.h"
#include "babstractcodeeditordocument_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QColor>
#include <QPoint>

/*============================================================================
================================ BSimpleCodeEditorDocumentPrivate ============
============================================================================*/

class B_CODEEDITOR_EXPORT BSimpleCodeEditorDocumentPrivate : public BAbstractCodeEditorDocumentPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSimpleCodeEditorDocument)
public:
    explicit BSimpleCodeEditorDocumentPrivate(BSimpleCodeEditorDocument *q, BCodeEditor *editor);
    ~BSimpleCodeEditorDocumentPrivate();
public:
    void init();
    QWidget *createEdit(QTextDocument **doc);
    bool eventFilter(QObject *obj, QEvent *e);
    bool keyPressEvent(QKeyEvent *e);
    void handleTab();
public Q_SLOTS:
    void cursorPositionChanged();
    void selectionChanged();
    void updatePasteAvailable(bool available);
public:
    QPlainTextEdit *ptedt;
    BLineNumberWidget *lnwgt;
    BeQt::TabWidth tabWidth;
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocumentPrivate)
};

#endif // BSIMPLECODEEDITORDOCUMENT_P_H
