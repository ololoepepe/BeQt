#ifndef BLOCALDOCUMENTDRIVER_P_H
#define BLOCALDOCUMENTDRIVER_P_H

class BAbstractFileType;

class QTextCodec;

#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QByteArray>
#include <QString>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BAbstractDocumentDriverPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
public:
    void init();
public:
    QString defaultDir;
    bool nativeLineEnd;
    QByteArray fileDialogState;
    BAbstractFileType *lastFileType;
private:
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
};

#endif // BLOCALDOCUMENTDRIVER_P_H
