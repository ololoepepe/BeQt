#ifndef BHELPBROWSER_P_H
#define BHELPBROWSER_P_H

#include "bclipboardnotifier.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BClipboardNotifierPrivate ===================
============================================================================*/

class BClipboardNotifierPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BClipboardNotifier)
public:
    explicit BClipboardNotifierPrivate(BClipboardNotifier *q);
    ~BClipboardNotifierPrivate();
public:
    void init();
    void detach();
public Q_SLOTS:
    void dataChanged();
public:
    bool textDataAvailable;
private:
    Q_DISABLE_COPY(BClipboardNotifierPrivate)
};

#endif // BHELPBROWSER_P_H
