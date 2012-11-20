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
    bool load(BCodeEditorDocument *doc, const QString &fileName);
    bool save(BCodeEditorDocument *doc, const QString &fileName);
};

#endif // BLOCALDOCUMENTDRIVER_H
