#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class BCodeEditorDocument;
class BLocalDocumentDriverPrivate;

class QObject;
class QWidget;
class QTextCodec;
class QStringList;
class QString;
class QByteArray;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>

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
protected:
    BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent = 0);
public:
    QString id() const;
    bool isBuisy() const;
    bool checkFileExistance(const QString &fileName);
    bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec);
    bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec);
    void setDefaultDir(const QString &dir);
    void setDialogState(const QByteArray &state);
    QByteArray dialogState() const;
protected:
    bool handleSaveOperation(const Operation &op);
    bool handleLoadOperation(const Operation &op);
private:
    Q_DISABLE_COPY(BLocalDocumentDriver)
};

#endif // BLOCALDOCUMENTDRIVER_H
