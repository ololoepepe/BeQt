#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BCodeEdit;

class QTextCodec;
class QWidget;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>
#include <QString>

/*============================================================================
================================ Code Editor Document
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocument : public BCodeEdit
{
    B_DECLARE_PRIVATE(BCodeEditorDocument)
    Q_OBJECT
public:
    explicit BCodeEditorDocument(QWidget *parent = 0);
    ~BCodeEditorDocument();
    //
    void setFileName(const QString &fn);
    void setCodec(QTextCodec *codec);
    void setCodec(const char *codecName);
    void setAsyncProcessingMinimumLength(int length);
    bool load( BAbstractDocumentDriver *driver, const QString &fileName = QString() );
    bool save( BAbstractDocumentDriver *driver, const QString &fileName = QString() );
    QString fileName() const;
    QTextCodec *codec() const;
    int asyncProcessingMinimumLength() const;
signals:
    void fileNameChanged(const QString &fn);
    void codecChanged(const QString &codecName);
    void loadingFinished(bool success);
    void savingFinished(bool success);
protected:
    BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H

