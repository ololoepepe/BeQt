#ifndef BHELPBROWSER_P_H
#define BHELPBROWSER_P_H

class BHelpBrowserPrivate;

class QVBoxLayout;
class QToolBar;
class QToolButton;
class QLabel;
class QLineEdit;
class QTextBrowser;
class QString;
class QTextCodec;

#include "bhelpbrowser.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QStringList>
#include <QObject>
#include <QMap>

/*============================================================================
================================ BHelpBrowserPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BHelpBrowserPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BHelpBrowser)
public:
    explicit BHelpBrowserPrivate( BHelpBrowser *q);
    ~BHelpBrowserPrivate();
public:
    void init();
public slots:
    void retranslateUi();
    void updateCaption();
    void search();
public:
    static QMap<QString, QStringList> searchCache;
public:
    QTextCodec *codec;
    QVBoxLayout *vlt;
      QToolBar *tbar;
        QToolButton *tbtnBackward;
        QToolButton *tbtnForward;
        //separator
        QToolButton *tbtnHome;
        //separator
        QLabel *lblSearch;
        QLineEdit *ledtSearch;
      QTextBrowser *tbrsr;
private:
    Q_DISABLE_COPY(BHelpBrowserPrivate)
};

#endif // BHELPBROWSER_P_H
