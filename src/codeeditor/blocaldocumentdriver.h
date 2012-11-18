#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class QObject;
class QString;
class QTextCodec;

#include "babstractdocumentdriver.h"

class BLocalDocumentDriver : public BAbstractDocumentDriver
{
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
    //
    QString id() const;
    bool load(const QString &fileName, QTextCodec *codec = 0);
    bool save(const QString &fileName, const QString &text, QTextCodec *codec = 0);
};

#endif // BLOCALDOCUMENTDRIVER_H
