#ifndef BDIALOG_P_H
#define BDIALOG_P_H

class QVBoxLayout;
class QDialogButtonBox;
class QWidget;

#include "bdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BDialogPrivate ==============================
============================================================================*/

class B_WIDGETS_EXPORT BDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BDialog)
public:
    explicit BDialogPrivate(BDialog *q);
    ~BDialogPrivate();
public:
    void init();
    void resetWidget(QWidget *w = 0);
public:
    QVBoxLayout *vlt;
    QDialogButtonBox *dlgbbox;
    QWidget *wgt;
private:
    Q_DISABLE_COPY(BDialogPrivate)
};

#endif // BDIALOG_P_H
