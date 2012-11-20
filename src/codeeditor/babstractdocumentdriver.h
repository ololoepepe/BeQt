#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class BCodeEditorDocument;
class BAbstractDocumentDriverPrivate;

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
        //
        Operation()
        {
            document = 0;
        }
        Operation(BCodeEditorDocument *doc, const QString &fn)
        {
            document = doc;
            fileName = fn;
        }
    };
    //
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
    //
    virtual QString id() const = 0;
    virtual bool load( BCodeEditorDocument *doc, const QString &fileName = QString() );
    virtual bool save( BCodeEditorDocument *doc, const QString &fileName = QString() );
    bool hasPendingLoadOperations() const;
    bool hasPendingSaveOperations() const;
    bool isDocumentInList(BCodeEditorDocument *doc) const;
signals:
    void newPendingLoadOperation();
    void newPendingSaveOperation();
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
protected:
    Operation nextPendingLoadOperation();
    Operation nextPendingSaveOperation();
    void emitLoadingFinished( const Operation &operation, bool success, const QString &text = QString() );
    void emitSavingFinished(const Operation &operation, bool success);
private:
    Q_DISABLE_COPY(BAbstractDocumentDriver)
};

#endif // BABSTRACTDOCUMENTDRIVER_H
