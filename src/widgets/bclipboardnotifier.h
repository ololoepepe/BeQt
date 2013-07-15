#ifndef BCLIPBOARDNOTIFIER_H
#define BCLIPBOARDNOTIFIER_H

class BClipboardNotifierPrivate;

class QWidget;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

class B_WIDGETS_EXPORT BClipboardNotifier : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BClipboardNotifier)
public:
    explicit BClipboardNotifier();
    ~BClipboardNotifier();
public:
    static BClipboardNotifier *instance();
public:
    bool textDataAvailable() const;
Q_SIGNALS:
    void textDataAvailableChanged(bool available);
protected:
    static BClipboardNotifier *_m_self;
private:
    Q_DISABLE_COPY(BClipboardNotifier)
};

#endif // BCLIPBOARDNOTIFIER_H
