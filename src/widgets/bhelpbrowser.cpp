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
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <QDebug>

/*============================================================================
================================ Help Browser Private Object
============================================================================*/

BHelpBrowserPrivateObject::BHelpBrowserPrivateObject(BHelpBrowserPrivate *p) :
    BBasePrivateObject(p)
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
    p_func()->retranslateUi();
}

void BHelpBrowserPrivateObject::sourceChanged()
{
    p_func()->updateCaption();
}

void BHelpBrowserPrivateObject::ledtSearchReturnPressed()
{
    p_func()->search();
}

/*============================================================================
================================ Help Browser Private
============================================================================*/

QMap<QString, QStringList> BHelpBrowserPrivate::searchCache;

//

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser *q, const QString &index, const QString &file,
                                         const QStringList &searchPaths) :
    BBasePrivate( *q, *new BHelpBrowserPrivateObject(this) )
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
          QObject::connect( ledtSearch, SIGNAL( returnPressed() ), o_func(), SLOT( ledtSearchReturnPressed() ) );
        tbar->addWidget(ledtSearch);
      vlt->addWidget(tbar);
      tbrsr = new QTextBrowser(q);
        QObject::connect( tbtnBackward, SIGNAL( clicked() ), tbrsr, SLOT( backward() ) );
        QObject::connect( tbtnForward, SIGNAL( clicked() ), tbrsr, SLOT( forward() ) );
        QObject::connect( tbtnHome, SIGNAL( clicked() ), tbrsr, SLOT( home() ) );
        QObject::connect( tbrsr, SIGNAL( backwardAvailable(bool) ), tbtnBackward, SLOT( setEnabled(bool) ) );
        QObject::connect( tbrsr, SIGNAL( forwardAvailable(bool) ), tbtnForward, SLOT( setEnabled(bool) ) );
        QObject::connect( tbrsr, SIGNAL( sourceChanged(QUrl) ), o_func(), SLOT( sourceChanged() ) );
      vlt->addWidget(tbrsr);
    //
    tbrsr->setSearchPaths(searchPaths);
    if ( !index.isEmpty() )
        tbrsr->setSource( QUrl(index) );
    if (file != index)
        tbrsr->setSource( QUrl(file) );
    tbrsr->setFocus();
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), o_func(), SLOT( languageChanged() ) );
}

BHelpBrowserPrivate::~BHelpBrowserPrivate()
{
    //
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

void BHelpBrowserPrivate::search()
{
    QString text = ledtSearch->text().toLower();
    if ( text.isEmpty() )
        return;
    QStringList sl = searchCache.value(text);
    if ( sl.isEmpty() )
    {
        foreach ( const QString &path, tbrsr->searchPaths() )
        {
            QDir dir(path);
            QStringList files = dir.entryList(QStringList() << "*.html" << "*.htm", QDir::Files);
            foreach (const QString &file, files)
            {
                if ( file.contains(text, Qt::CaseInsensitive) )
                    sl << file;
                QFile f( dir.absoluteFilePath(file) );
                if ( !f.open(QFile::ReadOnly) )
                    continue;
                QTextStream in(&f);
                //in.setCodec("UTF-8");
                QString ft = in.readAll();
                f.close();
                if ( ft.contains(text, Qt::CaseInsensitive) )
                    sl << file;
            }
        }
    }
    if ( sl.isEmpty() )
        return;
    if ( !searchCache.contains(text) )
        searchCache.insert(text, sl);
    QString source = "<center><font size=5><b>" + tr("Search results", "tbrsr text") + "</b></font></center><br><br>";
    foreach (const QString &file, sl)
        source += "<a href=\"" + file + "\">" + file + "</a><br>";
    source += "<br><br><a href=\"" + tbrsr->source().toString() + "\">" + tr("Back", "tbrsr text") + "</a>";
    tbrsr->setHtml(source);
}

//

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser &q, BHelpBrowserPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Help Browser
============================================================================*/

void BHelpBrowser::clearSearchCache()
{
    BHelpBrowserPrivate::searchCache.clear();
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
