class BCodeEdit;

#include "babstractdocumentdriver.h"

#include <QObject>

/*============================================================================
================================ Abstract Document Driver
============================================================================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(QObject *parent) :
    QObject(parent)
{
    //
}

BAbstractDocumentDriver::~BAbstractDocumentDriver()
{
    //
}

//

void BAbstractDocumentDriver::emitLoadingFinished(BCodeEdit *edit, bool success, const QString &text)
{
    emit loadingFinished(edit, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(BCodeEdit *edit, bool success)
{
    emit savingFinished(edit, success);
}
