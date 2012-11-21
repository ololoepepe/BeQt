#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class BCodeEditorDocument;
class BLocalDocumentDriverPrivate;

class QObject;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>

#include <QString>

/*============================================================================
================================ Local Document Driver
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriver : public BAbstractDocumentDriver
{
    B_DECLARE_PRIVATE(BLocalDocumentDriver)
    Q_OBJECT
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
    //
    QString id() const;
protected:
    BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BLocalDocumentDriver)
};

#endif // BLOCALDOCUMENTDRIVER_H
