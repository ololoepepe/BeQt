#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class BCodeEditorDocument;

class QObject;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>

#include <QString>

/*============================================================================
================================ Local Document Driver
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriver : public BAbstractDocumentDriver
{
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
    //
    QString id() const;
protected:
    bool load(BCodeEditorDocument *doc, bool *finished = 0, bool *success = 0, QString *text = 0);
    bool save(BCodeEditorDocument *doc, bool *finished = 0, bool *success = 0);
};

#endif // BLOCALDOCUMENTDRIVER_H
