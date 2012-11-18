#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BCodeEdit;

class QTextCodec;
class QString;
class QWidget;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>

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
    bool load(BAbstractDocumentDriver *driver);
    bool save(BAbstractDocumentDriver *driver);
    QString fileName() const;
    QTextCodec *codec() const;
    int asyncProcessingMinimumLength() const;
    bool isBuisy() const;
signals:
    void fileNameChanged(const QString &fn);
    void codecChanged(const QString &codecName);
    void loadingFinished(bool success);
    void savingFinished(bool success);
    void buisyChanged(bool buisy);
protected:
    BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H

