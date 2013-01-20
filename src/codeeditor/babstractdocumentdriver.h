#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class BCodeEditorPrivate;
class BCodeEditor;
class BCodeEditorDocument;
class BAbstractDocumentDriverPrivate;

class QWidget;
class QTextCodec;
class QByteArray;

#include <BeQtCore/BBase>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QtGlobal>

/*============================================================================
================================ BAbstractDocumentDriver =====================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractDocumentDriver : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAbstractDocumentDriver)
public:
    struct Operation
    {
        const BCodeEditorDocument *document;
        QString fileName;
        QTextCodec *codec;
    };
public:
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
protected:
    explicit BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent = 0);
public:
    virtual QString id() const = 0;
    virtual bool isBuisy() const = 0;
    virtual bool testFileExistance(const QString &fileName) = 0;
    virtual bool testFileReadOnly(const QString &fileName) = 0;
    virtual bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec) = 0;
    virtual bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec) = 0;
    virtual QByteArray saveState() const;
    virtual void restoreState(const QByteArray &state);
    bool load( BCodeEditorDocument *doc, const QString &fileName = QString() );
    bool load( BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString() );
    bool save( BCodeEditorDocument *doc, const QString &fileName = QString() );
    bool save( BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString() );
    BCodeEditor *editor() const;
protected:
    virtual bool handleSaveOperation(const Operation &op) = 0;
    virtual bool handleLoadOperation(const Operation &op) = 0;
    void emitLoadingFinished( const Operation &operation, bool success, const QString &text = QString() );
    void emitSavingFinished(const Operation &operation, bool success);
signals:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
private:
    Q_DISABLE_COPY(BAbstractDocumentDriver)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTDOCUMENTDRIVER_H
