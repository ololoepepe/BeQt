#include "babstractdocumentdriver.h"

#include <QObject>

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

void BAbstractDocumentDriver::emitLoadingFinished(bool success, const QString &text)
{
    emit loadingFinished(success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(bool success)
{
    emit savingFinished(success);
}
