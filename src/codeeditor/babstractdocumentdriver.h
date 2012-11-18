#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class BCodeEdit;

#include <QObject>
#include <QString>

/*============================================================================
================================ Abstract Document Driver
============================================================================*/

class BAbstractDocumentDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
    //
    virtual QString id() const = 0;
    virtual bool load( BCodeEdit *edit, const QString &fileName, const QString &codecName = QString() ) = 0;
    virtual bool save( BCodeEdit *edit, const QString &fileName,
                       const QString &text, const QString &codecName = QString() ) = 0;
signals:
    void loadingFinished(BCodeEdit *edit, bool success, const QString &text);
    void savingFinished(BCodeEdit *edit, bool success);
protected:
    void emitLoadingFinished(BCodeEdit *edit, bool success, const QString &text);
    void emitSavingFinished(BCodeEdit *edit, bool success);
};

#endif // BABSTRACTDOCUMENTDRIVER_H
