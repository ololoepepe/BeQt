#include "bcodeeditordocument.h"
#include "bcodeeditordocument_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeedit.h"
#include "bcodeedit_p.h"
#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QString>
#include <QTextCodec>
#include <QPointer>
#include <QByteArray>
#include <QMap>
#include <QTextDocument>

/*========== Code Editor Document Private Object ==========*/

BCodeEditorDocumentPrivateObject::BCodeEditorDocumentPrivateObject(BCodeEditorDocumentPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BCodeEditorDocumentPrivateObject::~BCodeEditorDocumentPrivateObject()
{
    //
}

//

void BCodeEditorDocumentPrivateObject::loadingFinished(bool success, const QString &text)
{
    _m_p->loadingFinished(static_cast<BAbstractDocumentDriver *>( sender() ), success, text);
}

void BCodeEditorDocumentPrivateObject::savingFinished(bool success)
{
    _m_p->savingFinished(static_cast<BAbstractDocumentDriver *>( sender() ), success);
}

/*========== Code Editor Document Private ==========*/

QMap<QString, BAbstractDocumentDriver *> BCodeEditorDocumentPrivate::createDriverMap(
        const QList<BAbstractDocumentDriver *> &drs)
{
    QMap<QString, BAbstractDocumentDriver *> m;
    foreach (BAbstractDocumentDriver *dr, drs)
    {
        QString id = dr->id();
        if ( !id.contains(id) )
            m.insert(id, dr);
    }
    return m;
}

//

BCodeEditorDocumentPrivate::BCodeEditorDocumentPrivate(BCodeEditorDocument *q,
                                                       const QList<BAbstractDocumentDriver *> &drs) :
    BBasePrivate(q), _m_o( new BCodeEditorDocumentPrivateObject(this) ), Drivers( createDriverMap(drs) )
{
    foreach (BAbstractDocumentDriver *dr, drs)
    {
        QObject::connect( dr, SIGNAL( loadingFinished(bool, QString) ), _m_o, SLOT( loadingFinished(bool, QString) ) );
        QObject::connect( dr, SIGNAL( savingFinished(bool) ), _m_o, SLOT( savingFinished(bool) ) );
    }
    codec = QTextCodec::codecForName("UTF-8");
    cedt = new BCodeEdit;
    buisy = false;
    QObject::connect( cedt.data(), SIGNAL( modificationChanged(bool) ), q, SIGNAL( modificationChanged(bool) ) );
}

BCodeEditorDocumentPrivate::~BCodeEditorDocumentPrivate()
{
    foreach (BAbstractDocumentDriver *drv, Drivers)
        delete drv;
    if ( !cedt.isNull() )
        cedt->deleteLater();
    _m_o->deleteLater();
}

//

void BCodeEditorDocumentPrivate::loadingFinished(BAbstractDocumentDriver *driver, bool success, const QString &text)
{
    if (success)
    {
        cedt->setText(text);
        cedt->innerEdit()->document()->setModified(false);
    }
    QMetaObject::invokeMethod( q_func(), "loadingFinished", Q_ARG(bool, success) );
    buisy = false;
    QMetaObject::invokeMethod( q_func(), "buisyChanged", Q_ARG(bool, false) );
}

void BCodeEditorDocumentPrivate::savingFinished(BAbstractDocumentDriver *driver, bool success)
{
    if (success)
        cedt->innerEdit()->document()->setModified(false);
    QMetaObject::invokeMethod( q_func(), "savingFinished", Q_ARG(bool, success) );
    buisy = false;
    QMetaObject::invokeMethod( q_func(), "buisyChanged", Q_ARG(bool, false) );
}

/*========== Code Editor Document ==========*/

BCodeEditorDocument::BCodeEditorDocument(const QList<BAbstractDocumentDriver *> &drivers, QObject *parent) :
    QObject(parent), BBase( *new BCodeEditorDocumentPrivate(this, drivers) )
{
    //
}

BCodeEditorDocument::~BCodeEditorDocument()
{
    //
}

void BCodeEditorDocument::setFileName(const QString &fn)
{
    if ( isBuisy() )
        return;
    B_D(BCodeEditorDocument);
    bool b = (fn == d->fileName);
    d->fileName = fn;
    if (b)
        emit fileNameChanged(fn);
}

void BCodeEditorDocument::setCodec(QTextCodec *codec)
{
    if ( isBuisy() )
        return;
    if (!codec)
        return;
    B_D(BCodeEditorDocument);
    bool b = (codec == d->codec);
    d->codec = codec;
    if (b)
        emit codecChanged( QString::fromLatin1( codec->name() ) );
}

void BCodeEditorDocument::setCodec(const char *codecName)
{
    setCodec( QTextCodec::codecForName(codecName) );
}

bool BCodeEditorDocument::load(const QString &driverId)
{
    if ( isBuisy() )
        return false;
    BAbstractDocumentDriver *dr = d_func()->Drivers.value(driverId);
    if (dr)
        return false;
    B_D(BCodeEditorDocument);
    d->buisy = true;
    emit buisyChanged(true);
    bool b = dr->load(d->fileName, d->codec);
    if (!b)
        d->buisy = false;
    return b;
}

bool BCodeEditorDocument::save(const QString &driverId)
{
    if ( isBuisy() )
        return false;
    BAbstractDocumentDriver *dr = d_func()->Drivers.value(driverId);
    if (dr)
        return false;
    B_D(BCodeEditorDocument);
    d->buisy = true;
    emit buisyChanged(true);
    bool b = dr->save(d->fileName, d->cedt->text(), d->codec);
    if (!b)
        d->buisy = false;
    return b;
}

QString BCodeEditorDocument::fileName() const
{
    return d_func()->fileName;
}

QTextCodec *BCodeEditorDocument::codec() const
{
    return d_func()->codec;
}

bool BCodeEditorDocument::isModified() const
{
    return d_func()->cedt->isModified();
}

bool BCodeEditorDocument::isBuisy() const
{
    return d_func()->buisy;
}

BCodeEdit *BCodeEditorDocument::editWidget() const
{
   return d_func()->cedt;
}

//

BCodeEditorDocument::BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}

