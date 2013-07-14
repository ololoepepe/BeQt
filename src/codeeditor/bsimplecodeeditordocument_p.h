#ifndef BSIMPLECODEEDITORDOCUMENT_P_H
#define BSIMPLECODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BLineNumberWidget;

class QTextDocument;
class QWidget;
class QPlainTextEdit;
class QPoint;

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
public Q_SLOTS:
    void cursorPositionChanged();
    void selectionChanged();
    void updatePasteAvailable(bool available);
    void popupMenu(const QPoint &pos);
public:
    QPlainTextEdit *ptedt;
    BLineNumberWidget *lnwgt;
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocumentPrivate)
};

#endif // BSIMPLECODEEDITORDOCUMENT_P_H
