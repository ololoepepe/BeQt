#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BAbstractFileType;

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
    void setFileType(BAbstractFileType *ft);
    bool load(BAbstractDocumentDriver *driver, const QString &fileName = QString(), QTextCodec *codec = 0);
    bool save(BAbstractDocumentDriver *driver, const QString &fileName = QString(), QTextCodec *codec = 0);
    QString fileName() const;
    QTextCodec *codec() const;
    int asyncProcessingMinimumLength() const;
    BAbstractFileType *fileType() const;
signals:
    void fileNameChanged(const QString &fn);
    void codecChanged(const QString &codecName);
    void loadingFinished(bool success);
    void savingFinished(bool success);
    void fileTypeChanged(BAbstractFileType *ft);
protected:
    BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H

