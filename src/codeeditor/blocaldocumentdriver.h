#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class BCodeEdit;

class QObject;

#include "babstractdocumentdriver.h"

#include <QString>

/*============================================================================
================================ Local Document Driver
============================================================================*/

class BLocalDocumentDriver : public BAbstractDocumentDriver
{
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
    //
    QString id() const;
    bool load( BCodeEdit *edit, const QString &fileName, const QString &codecName = QString() );
    bool save( BCodeEdit *edit, const QString &fileName, const QString &text, const QString &codecName = QString() );
};

#endif // BLOCALDOCUMENTDRIVER_H
