#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BSplittedLinesDialog;
class BCodeEditor;
class BCodeEdit;

class QTextDocument;
class QWidget;

#include "bcodeeditordocument.h"
#include "babstractcodeeditordocument_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QObject>

/*============================================================================
================================ BCodeEditorDocumentPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocumentPrivate : public BAbstractCodeEditorDocumentPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEditorDocument)
public:
    explicit BCodeEditorDocumentPrivate(BCodeEditorDocument *q, BCodeEditor *editor);
    ~BCodeEditorDocumentPrivate();
public:
    void init();
    QWidget *createEdit(QTextDocument **doc);
public:
    BCodeEdit *cedt;
    BSplittedLinesDialog *sld;
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H
