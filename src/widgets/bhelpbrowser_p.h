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

class B_WIDGETS_EXPORT BHelpBrowserPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BHelpBrowserPrivateObject(BHelpBrowserPrivate *p);
    ~BHelpBrowserPrivateObject();
    //
    BHelpBrowserPrivate *const _m_p;
public slots:
    void languageChanged();
    void sourceChanged();
private:
    Q_DISABLE_COPY(BHelpBrowserPrivateObject)
};

class B_WIDGETS_EXPORT BHelpBrowserPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BHelpBrowser)
    B_DECLARE_PUBLIC(BHelpBrowser)
public:
    BHelpBrowserPrivate( BHelpBrowser *q, const QString &index, const QString &file,
                         const QStringList &searchPaths = QStringList() );
    ~BHelpBrowserPrivate();
    //
    void retranslateUi();
    void updateCaption();
    //
    BHelpBrowserPrivateObject *const _m_o;
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
private:
    Q_DISABLE_COPY(BHelpBrowserPrivate)
    //
    friend class BHelpBrowserPrivateObject;
};

#endif // BHELPBROWSER_P_H
