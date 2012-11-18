#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class QString;
class QTextCodec;

#include <QObject>

class BAbstractDocumentDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
    //
    virtual QString id() const = 0;
    virtual bool load(const QString &fileName, QTextCodec *codec = 0) = 0;
    virtual bool save(const QString &fileName, const QString &text, QTextCodec *codec = 0) = 0;
signals:
    void loadingFinished(bool success, const QString &text);
    void savingFinished(bool success);
protected:
    void emitLoadingFinished(bool success, const QString &text);
    void emitSavingFinished(bool success);
};

#endif // BABSTRACTDOCUMENTDRIVER_H
