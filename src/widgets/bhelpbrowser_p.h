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

#include "bhelpbrowser.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QStringList>
#include <QObject>
#include <QMap>

/*============================================================================
================================ Help Browser Private
============================================================================*/

class B_WIDGETS_EXPORT BHelpBrowserPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BHelpBrowser)
    Q_OBJECT
public:
    static QMap<QString, QStringList> searchCache;
    //
    BHelpBrowserPrivate( BHelpBrowser *q, const QString &index, const QString &file,
                         const QStringList &searchPaths = QStringList() );
    ~BHelpBrowserPrivate();
    //
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
public slots:
    void retranslateUi();
    void updateCaption();
    void search();
private:
    Q_DISABLE_COPY(BHelpBrowserPrivate)
};

#endif // BHELPBROWSER_P_H
