#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BSplittedLinesDialog;
class BCodeEditor;

class QString;
class QTextCodec;

#include "bcodeeditordocument.h"
#include "bcodeedit_p.h"
#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

/*============================================================================
================================ BCodeEditorDocumentPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocumentPrivate : public BCodeEditPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEditorDocument)
public:
    explicit BCodeEditorDocumentPrivate(BCodeEditorDocument *q, BCodeEditor *edr);
    ~BCodeEditorDocumentPrivate();
public:
    void init();
    void setFileName(QString fn);
    void setCodec(QTextCodec *c);
public Q_SLOTS:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
public:
    BCodeEditor *const Editor;
public:
    QString fileName;
    QTextCodec *codec;
    int asyncMin;
    BSplittedLinesDialog *sld;
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H
