#ifndef BSIMPLECODEEDITORDOCUMENT_P_H
#define BSIMPLECODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BLineNumberWidget;

class QTextDocument;
class QWidget;
class QPlainTextEdit;

#include "bsimplecodeeditordocument.h"
#include "babstractcodeeditordocument_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QColor>

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
    void highlightCurrentLine();
public Q_SLOTS:
    void cursorPositionChanged();
public:
    QPlainTextEdit *ptedt;
    BLineNumberWidget *lnwgt;
    bool highlightLine;
    QColor lineColor;
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocumentPrivate)
};

#endif // BSIMPLECODEEDITORDOCUMENT_P_H
