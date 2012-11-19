#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BCodeEdit;

class QString;
class QTextCodec;

#include "bcodeeditordocument.h"
#include "bcodeedit_p.h"

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
    QString fileName;
    QTextCodec *codec;
    int asyncMin;
    bool buisy;
public slots:
    void loadingFinished(BCodeEditorDocument *doc, bool success, const QString &text);
    void savingFinished(BCodeEditorDocument *doc, bool success);
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H

