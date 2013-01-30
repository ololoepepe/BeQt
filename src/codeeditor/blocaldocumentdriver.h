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
================================ BLocalDocumentDriver ========================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriver : public BAbstractDocumentDriver
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocalDocumentDriver)
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
protected:
    explicit BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent = 0);
public:
    QString id() const;
    bool isBuisy() const;
    bool testFileExistance(const QString &fileName);
    bool testFileReadOnly(const QString &fileName);
    bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec);
    bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec);
    QByteArray saveState() const;
    void restoreState(const QByteArray &state);
    void setDefaultDir(const QString &dir);
    void setNativeLineEnd(bool enabled);
protected:
    bool handleSaveOperation(const Operation &op);
    bool handleLoadOperation(const Operation &op);
private:
    Q_DISABLE_COPY(BLocalDocumentDriver)
};

#endif // BLOCALDOCUMENTDRIVER_H
