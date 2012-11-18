#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEditorDocumentPrivate;
class BAbstractDocumentDriver;
class BCodeEdit;

class QString;
class QTextCodec;

#include "bcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QList>
#include <QPointer>

class BCodeEditorDocumentPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BCodeEditorDocumentPrivateObject(BCodeEditorDocumentPrivate *p);
    ~BCodeEditorDocumentPrivateObject();
    //
    BCodeEditorDocumentPrivate *const _m_p;
public slots:
    void loadingFinished(bool success, const QString &text);
    void savingFinished(bool success);
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivateObject)
};

class BCodeEditorDocumentPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditorDocument)
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
    BCodeEditorDocumentPrivateObject *const _m_o;
    const QMap<QString, BAbstractDocumentDriver *> Drivers;
    //
    QString fileName;
    QTextCodec *codec;
    QPointer<BCodeEdit> cedt;
    bool buisy;
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
    //
    friend class BCodeEditorDocumentPrivateObject;
};

#endif // BCODEEDITORDOCUMENT_P_H

