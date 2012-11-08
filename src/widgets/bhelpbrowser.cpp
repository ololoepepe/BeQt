#include "bhelpbrowser.h"
#include "bhelpbrowser_p.h"
#include "bapplication.h"

#include <QApplication>
#include <QWidget>
#include <QString>
#include <QUrl>
#include <QStringList>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QLineEdit>

BHelpBrowserPrivateObject::BHelpBrowserPrivateObject(BHelpBrowserPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BHelpBrowserPrivateObject::~BHelpBrowserPrivateObject()
{
    //
}

//

void BHelpBrowserPrivateObject::languageChanged()
{
    _m_p->retranslateUi();
}

void BHelpBrowserPrivateObject::sourceChanged()
{
    _m_p->updateCaption();
}

//

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser *q, const QString &index, const QString &file,
                                         const QStringList &searchPaths) :
    BBasePrivate(q), _m_o( new BHelpBrowserPrivateObject(this) )
{
    QVBoxLayout *vlt = new QVBoxLayout(q);
      tbar = new QToolBar(q);
        tbtnBackward = new QToolButton;
          tbtnBackward->setEnabled(false);
          tbtnBackward->setIcon( BApplication::beqtIcon("back") );
        tbar->addWidget(tbtnBackward);
        tbtnForward = new QToolButton;
          tbtnForward->setEnabled(false);
          tbtnForward->setIcon( BApplication::beqtIcon("forward") );
        tbar->addWidget(tbtnForward);
        tbar->addSeparator();
        tbtnHome = new QToolButton;
          tbtnHome->setIcon( BApplication::beqtIcon("gohome") );
        tbar->addWidget(tbtnHome);
        tbar->addSeparator();
        lblSearch = new QLabel;
        tbar->addWidget(lblSearch);
        ledtSearch = new QLineEdit;
        tbar->addWidget(ledtSearch);
      vlt->addWidget(tbar);
      tbrsr = new QTextBrowser(q);
        QObject::connect( tbtnBackward, SIGNAL( clicked() ), tbrsr, SLOT( backward() ) );
        QObject::connect( tbtnForward, SIGNAL( clicked() ), tbrsr, SLOT( forward() ) );
        QObject::connect( tbtnHome, SIGNAL( clicked() ), tbrsr, SLOT( home() ) );
        QObject::connect( tbrsr, SIGNAL( backwardAvailable(bool) ), tbtnBackward, SLOT( setEnabled(bool) ) );
        QObject::connect( tbrsr, SIGNAL( forwardAvailable(bool) ), tbtnForward, SLOT( setEnabled(bool) ) );
        QObject::connect( tbrsr, SIGNAL( sourceChanged(QUrl) ), _m_o, SLOT( sourceChanged() ) );
      vlt->addWidget(tbrsr);
    //
    tbrsr->setSearchPaths(searchPaths);
    if ( !index.isEmpty() )
        tbrsr->setSource( QUrl(index) );
    if (file != index)
        tbrsr->setSource( QUrl(file) );
    tbrsr->setFocus();
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), _m_o, SLOT( languageChanged() ) );
}

BHelpBrowserPrivate::~BHelpBrowserPrivate()
{
    _m_o->deleteLater();
}

//

void BHelpBrowserPrivate::retranslateUi()
{
    tbtnBackward->setToolTip( tr("Backward", "tbtn toolTip") );
    tbtnForward->setToolTip( tr("Forward", "tbtn toolTip") );
    tbtnHome->setToolTip( tr("Home", "tbtn toolTip") );
    lblSearch->setText(tr("Search:", "lbl text") + " ");
    updateCaption();
}

void BHelpBrowserPrivate::updateCaption()
{
    QString title = tr("Help", "windowTitle");
    QString dt = tbrsr->documentTitle();
    if ( !dt.isEmpty() )
        title += ": " + dt;
    q_func()->setWindowTitle(title);
}

//

BHelpBrowser::BHelpBrowser(QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, "", "") )
{
    //
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, "", "", searchPaths) )
{
    //
}

BHelpBrowser::BHelpBrowser(const QString &file, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, "", file) )
{
    //
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &file, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, "", file, searchPaths) )
{
    //
}

BHelpBrowser::BHelpBrowser(const QString &index, const QString &file, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, index, file) )
{
    //
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &index, const QString &file,
                           QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this, index, file, searchPaths) )
{
    //
}

BHelpBrowser::~BHelpBrowser()
{
    //
}

//

void BHelpBrowser::setSearchPaths(const QStringList &paths)
{
    d_func()->tbrsr->setSearchPaths(paths);
}

void BHelpBrowser::setFile(const QString &file)
{
    d_func()->tbrsr->setSource( QUrl(file) );
}

//

BHelpBrowser::BHelpBrowser(BHelpBrowserPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}
