#include "bclipboardnotifier.h"
#include "bclipboardnotifier_p.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QApplication>
#include <QObject>
#include <QClipboard>
#include <QMetaObject>

static void clipboardNotifierCleanup()
{
    delete BClipboardNotifier::instance();
}

/*============================================================================
================================ BClipboardNotifierPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BClipboardNotifierPrivate::BClipboardNotifierPrivate(BClipboardNotifier *q) :
    BBasePrivate(q)
{
    //
}

BClipboardNotifierPrivate::~BClipboardNotifierPrivate()
{
    detach();
}

/*============================== Public methods ============================*/

void BClipboardNotifierPrivate::init()
{
    textDataAvailable = !QApplication::clipboard()->text().isEmpty();
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

void BClipboardNotifierPrivate::detach()
{
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

/*============================== Public slots ==============================*/

void BClipboardNotifierPrivate::dataChanged()
{
    bool b = !QApplication::clipboard()->text().isEmpty();
    bool bb = (b != textDataAvailable);
    textDataAvailable = b;
    if (bb)
        QMetaObject::invokeMethod(q_func(), "textDataAvailableChanged", Q_ARG(bool, b));
}

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BClipboardNotifier::BClipboardNotifier() :
    QObject(0), BBase(*new BClipboardNotifierPrivate(this))
{
    bTest(!_m_self, "BClipboardNotifier", "There should be only one instance of BClipboardNotifier");
    if (_m_self)
        _m_self->d_func()->detach();
    else
        qAddPostRoutine(&clipboardNotifierCleanup);
    d_func()->init();
    _m_self = this;
}

BClipboardNotifier::~BClipboardNotifier()
{
    _m_self = 0;
}

/*============================== Static public methods =====================*/

BClipboardNotifier *BClipboardNotifier::instance()
{
    return _m_self;
}

/*============================== Public methods ============================*/

bool BClipboardNotifier::textDataAvailable() const
{
    return d_func()->textDataAvailable;
}

/*============================== Static protected variables ================*/

BClipboardNotifier *BClipboardNotifier::_m_self = 0;
