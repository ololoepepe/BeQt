#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class BCodeEditorPrivate;
class BCodeEditor;
class BCodeEditorDocument;
class BAbstractDocumentDriverPrivate;

class QWidget;
class QTextCodec;

#include <BeQtCore/BBase>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QtGlobal>

/*============================================================================
================================ Abstract Document Driver
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractDocumentDriver : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BAbstractDocumentDriver)
    Q_OBJECT
public:
    struct Operation
    {
        BCodeEditorDocument *document;
        QString fileName;
        QTextCodec *codec;
    };
    //
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
    //
    virtual QString id() const = 0;
    virtual bool isBuisy() const = 0;
    virtual bool checkFileExistance(const QString &fileName) = 0;
    virtual bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec) = 0;
    virtual bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec) = 0;
    bool load( BCodeEditorDocument *doc, const QString &fileName = QString() );
    bool load( BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString() );
    bool save( BCodeEditorDocument *doc, const QString &fileName = QString() );
    bool save( BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString() );
    BCodeEditor *editor() const;
signals:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
protected:
    BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent = 0);
    //
    virtual bool handleSaveOperation(const Operation &op) = 0;
    virtual bool handleLoadOperation(const Operation &op) = 0;
    void emitLoadingFinished( const Operation &operation, bool success, const QString &text = QString() );
    void emitSavingFinished(const Operation &operation, bool success);
private:
    Q_DISABLE_COPY(BAbstractDocumentDriver)
    //
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTDOCUMENTDRIVER_H
