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
================================ Help Browser Private
============================================================================*/

QMap<QString, QStringList> BHelpBrowserPrivate::searchCache;

//

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser *q, const QString &index, const QString &file,
                                         const QStringList &searchPaths) :
    BBasePrivate(q)
{
    QVBoxLayout *vlt = new QVBoxLayout(q);
      tbar = new QToolBar(q);
        tbtnBackward = new QToolButton;
          tbtnBackward->setEnabled(false);
          tbtnBackward->setIcon( BApplication::icon("back") );
        tbar->addWidget(tbtnBackward);
        tbtnForward = new QToolButton;
          tbtnForward->setEnabled(false);
          tbtnForward->setIcon( BApplication::icon("forward") );
        tbar->addWidget(tbtnForward);
        tbar->addSeparator();
        tbtnHome = new QToolButton;
          tbtnHome->setIcon( BApplication::icon("gohome") );
        tbar->addWidget(tbtnHome);
        tbar->addSeparator();
        lblSearch = new QLabel;
        tbar->addWidget(lblSearch);
        ledtSearch = new QLineEdit;
          QObject::connect( ledtSearch, SIGNAL( returnPressed() ), this, SLOT( search() ) );
        tbar->addWidget(ledtSearch);
      vlt->addWidget(tbar);
      tbrsr = new QTextBrowser(q);
        connect( tbtnBackward, SIGNAL( clicked() ), tbrsr, SLOT( backward() ) );
        connect( tbtnForward, SIGNAL( clicked() ), tbrsr, SLOT( forward() ) );
        connect( tbtnHome, SIGNAL( clicked() ), tbrsr, SLOT( home() ) );
        connect( tbrsr, SIGNAL( backwardAvailable(bool) ), tbtnBackward, SLOT( setEnabled(bool) ) );
        connect( tbrsr, SIGNAL( forwardAvailable(bool) ), tbtnForward, SLOT( setEnabled(bool) ) );
        connect( tbrsr, SIGNAL( sourceChanged(QUrl) ), this, SLOT( updateCaption() ) );
      vlt->addWidget(tbrsr);
    //
    tbrsr->setSearchPaths(searchPaths);
    if ( !index.isEmpty() )
        tbrsr->setSource( QUrl(index) );
    if (file != index)
        tbrsr->setSource( QUrl(file) );
    tbrsr->setFocus();
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
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
