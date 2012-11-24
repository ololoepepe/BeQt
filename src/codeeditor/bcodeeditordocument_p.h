#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BAbstractFileType;

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
================================ Code Editor Document Private
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocumentPrivate : public BCodeEditPrivate
{
    B_DECLARE_PUBLIC(BCodeEditorDocument)
    Q_OBJECT
public:
    explicit BCodeEditorDocumentPrivate(BCodeEditorDocument *q);
    ~BCodeEditorDocumentPrivate();
    //
    void init();
    void setFileName(const QString &fn);
    void setCodec(QTextCodec *c);
    //
    QString fileName;
    QTextCodec *codec;
    int asyncMin;
    BAbstractFileType *fileType;
public slots:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H

