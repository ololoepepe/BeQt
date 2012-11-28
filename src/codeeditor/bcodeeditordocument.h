#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BAbstractFileType;
class BCodeEditorPrivate;

class QTextCodec;
class QWidget;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>
#include <QString>

/*============================================================================
================================ BCodeEditorDocument =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocument : public BCodeEdit
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEditorDocument)
public:
    explicit BCodeEditorDocument(QWidget *parent = 0);
    ~BCodeEditorDocument();
protected:
    explicit BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    void setFileName(const QString &fn);
    void setCodec(QTextCodec *codec);
    void setCodec(const char *codecName);
    void setAsyncProcessingMinimumLength(int length);
    void setFileType(BAbstractFileType *ft);
    bool load( BAbstractDocumentDriver *driver, const QString &fileName = QString() );
    bool load( BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString() );
    bool save( BAbstractDocumentDriver *driver, const QString &fileName = QString() );
    bool save( BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString() );
    QString fileName() const;
    QTextCodec *codec() const;
    QString codecName() const;
    int asyncProcessingMinimumLength() const;
    BAbstractFileType *fileType() const;
signals:
    void fileNameChanged(const QString &fn);
    void codecChanged(const QString &codecName);
    void loadingFinished(bool success);
    void savingFinished(bool success);
    void fileTypeChanged(BAbstractFileType *ft);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
    friend class BCodeEditorPrivate;
};

#endif // BCODEEDITORDOCUMENT_H

