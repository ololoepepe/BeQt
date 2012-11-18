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
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
    //
    virtual QString id() const = 0;
    bool enqueueLoadOperation(BCodeEditorDocument *doc, bool *finished = 0);
    bool enqueueSaveOperation(BCodeEditorDocument *doc, bool *finished = 0);
    bool hasPendingLoadOperations() const;
    bool hasPendingSaveOperations() const;
signals:
    void newPendingLoadOperation();
    void newPendingSaveOperation();
    void loadingFinished(BCodeEditorDocument *doc, bool success, const QString &text);
    void savingFinished(BCodeEditorDocument *doc, bool success);
protected:
    virtual bool load(BCodeEditorDocument *doc, bool *finished = 0);
    virtual bool save(BCodeEditorDocument *doc, bool *finished = 0);
    BCodeEditorDocument *nextPendingLoadOperation();
    BCodeEditorDocument *nextPendingSaveOperation();
    void emitLoadingFinished(BCodeEditorDocument *doc, bool success, const QString &text);
    void emitSavingFinished(BCodeEditorDocument *doc, bool success);
private:
    Q_DISABLE_COPY(BAbstractDocumentDriver)
};

#endif // BABSTRACTDOCUMENTDRIVER_H
