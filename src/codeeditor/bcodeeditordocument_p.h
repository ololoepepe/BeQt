#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BCodeEdit;

class QString;
class QTextCodec;

#include "bcodeeditordocument.h"
#include "bcodeedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QList>
#include <QPointer>

class BCodeEditorDocumentPrivateObject : public BCodeEditPrivateObject
{
    B_DECLARE_PRIVATE_O(BCodeEditorDocument)
    Q_OBJECT
public:
    explicit BCodeEditorDocumentPrivateObject(BCodeEditorDocumentPrivate *p);
    ~BCodeEditorDocumentPrivateObject();
public slots:
    void loadingFinished(bool success, const QString &text);
    void savingFinished(bool success);
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivateObject)
};

class BCodeEditorDocumentPrivate : public BCodeEditPrivate
{
    B_DECLARE_PUBLIC(BCodeEditorDocument)
    B_DECLARE_OBJECT(BCodeEditorDocument)
    Q_DECLARE_TR_FUNCTIONS(BCodeEditorDocument)
public:
    static QMap<QString, BAbstractDocumentDriver *> createDriverMap(const QList<BAbstractDocumentDriver *> &drs);
    //
    explicit BCodeEditorDocumentPrivate(BCodeEditorDocument *q, const QList<BAbstractDocumentDriver *> &drs);
    ~BCodeEditorDocumentPrivate();
    //
    void loadingFinished(BAbstractDocumentDriver *driver, bool success, const QString &text);
    void savingFinished(BAbstractDocumentDriver *driver, bool success);
    //
    const QMap<QString, BAbstractDocumentDriver *> Drivers;
    //
    QString fileName;
    QTextCodec *codec;
    bool buisy;
protected:
    BCodeEditorDocumentPrivate(BCodeEditorDocument &q, BCodeEditorDocumentPrivateObject &o);
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H

