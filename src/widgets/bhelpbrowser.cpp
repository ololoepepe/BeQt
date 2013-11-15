#include "bhelpbrowser.h"
#include "bhelpbrowser_p.h"
#include "bapplication.h"

#include <BeQtCore/BTextTools>
#include <BeQtCore/BDirTools>
#include <BeQtCore/BTextMatch>
#include <BeQtCore/BTextMatchList>

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
#include <QTextCodec>
#include <QRegExp>
#include <QDesktopServices>
#include <QPushButton>

#include <QDebug>

/*============================================================================
================================ BHelpBrowserPrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser *q) :
    BBasePrivate(q)
{
    //
}

BHelpBrowserPrivate::~BHelpBrowserPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BHelpBrowserPrivate::init()
{
    codec = QTextCodec::codecForName("UTF-8");
    B_Q(BHelpBrowser);
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
          connect( ledtSearch, SIGNAL( returnPressed() ), this, SLOT( search() ) );
        tbar->addWidget(ledtSearch);
        btnFind = new QPushButton;
          connect(btnFind, SIGNAL(clicked()), this, SLOT(search()));
          connect(btnFind, SIGNAL(clicked()), ledtSearch, SLOT(setFocus()));
        tbar->addWidget(btnFind);
      vlt->addWidget(tbar);
      tbrsr = new QTextBrowser(q);
        connect( tbtnBackward, SIGNAL( clicked() ), tbrsr, SLOT( backward() ) );
        connect( tbtnForward, SIGNAL( clicked() ), tbrsr, SLOT( forward() ) );
        connect( tbtnHome, SIGNAL( clicked() ), tbrsr, SLOT( home() ) );
        connect( tbrsr, SIGNAL( backwardAvailable(bool) ), tbtnBackward, SLOT( setEnabled(bool) ) );
        connect( tbrsr, SIGNAL( forwardAvailable(bool) ), tbtnForward, SLOT( setEnabled(bool) ) );
        connect( tbrsr, SIGNAL( sourceChanged(QUrl) ), this, SLOT( updateCaption() ) );
        tbrsr->setOpenExternalLinks(true);
      vlt->addWidget(tbrsr);
    //
    tbrsr->setFocus();
    retranslateUi();
    QObject::connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

/*============================== Public slots ==============================*/

void BHelpBrowserPrivate::retranslateUi()
{
    tbtnBackward->setToolTip( tr("Back", "tbtn toolTip") );
    tbtnForward->setToolTip( tr("Forward", "tbtn toolTip") );
    tbtnHome->setToolTip( tr("Home", "tbtn toolTip") );
    lblSearch->setText(tr("Search:", "lbl text") + " ");
    btnFind->setText(tr("Find", "btn text"));
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
    if (text.isEmpty())
        return;
    QStringList sl;
    if (!searchCache.contains(text))
    {
        foreach (const QString &path, tbrsr->searchPaths())
        {
            QStringList files = BDirTools::entryList(path, QStringList() << "*.html" << "*.htm", QDir::Files);
            foreach (const QString &file, files)
                if (BDirTools::readTextFile(file, codec).contains(text, Qt::CaseInsensitive))
                    sl << file;
        }
    }
    else
    {
        sl = searchCache.value(text);
    }
    if (!searchCache.contains(text))
        searchCache.insert(text, sl);
    QString source = "<center><font size=5><b>" + tr("Search results", "tbrsr text") + "</b></font></center><br><br>";
    if (!sl.isEmpty())
    {
        foreach (const QString &file, sl)
        {
            QString s = BDirTools::readTextFile(file, codec);
            BTextMatchList ml = BTextTools::match(s, QRegExp(".+"), QRegExp("<title>(\n)*", Qt::CaseInsensitive),
                                                  QRegExp("(\n)*</title>", Qt::CaseInsensitive));
            if (ml.isEmpty())
                continue;
            s = ml.first();
            source += "<a href=\"" + QFileInfo(file).fileName() + "\">" + s + "</a><br>";
        }
    }
    else
    {
        source += "<img src=\"" + BDirTools::findResource("beqt/pixmaps/sadpanda.jpg", BDirTools::GlobalOnly) + "\">";
        source += "<br><br>" + tr("Sorry, nothing found.", "tbrsr text");
    }
    source += "<br><br><a href=\"" + tbrsr->source().toString() + "\">" + tr("Back", "tbrsr text") + "</a>";
    tbrsr->setHtml(source);
}

/*============================== Static public variables ===================*/

QMap<QString, QStringList> BHelpBrowserPrivate::searchCache;

/*============================================================================
================================ BHelpBrowser ================================
============================================================================*/

/*============================== Public constructors =======================*/

BHelpBrowser::BHelpBrowser(QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this) )
{
    d_func()->init();
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this) )
{
    d_func()->init();
    d_func()->tbrsr->setSearchPaths(searchPaths);
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &file, QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this) )
{
    d_func()->init();
    B_D(BHelpBrowser);
    d->tbrsr->setSearchPaths(searchPaths);
    d->tbrsr->setSource( QUrl(file) );
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &index, const QString &file,
                           QWidget *parent) :
    QWidget(parent), BBase( *new BHelpBrowserPrivate(this) )
{
    d_func()->init();
    B_D(BHelpBrowser);
    d->tbrsr->setSearchPaths(searchPaths);
    if ( !index.isEmpty() )
        d->tbrsr->setSource( QUrl(index) );
    if (file != index)
        d->tbrsr->setSource( QUrl(file) );
}

BHelpBrowser::~BHelpBrowser()
{
    //
}

/*============================== Protected constructors ====================*/

BHelpBrowser::BHelpBrowser(BHelpBrowserPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

void BHelpBrowser::clearSearchCache()
{
    BHelpBrowserPrivate::searchCache.clear();
}

/*============================== Public methods ============================*/

void BHelpBrowser::setSearchPaths(const QStringList &paths)
{
    d_func()->tbrsr->setSearchPaths(paths);
}

void BHelpBrowser::setFile(const QString &file)
{
    d_func()->tbrsr->setSource( QUrl(file) );
}

void BHelpBrowser::setCodec(QTextCodec *codec)
{
    if (!codec)
        return;
    d_func()->codec = codec;
}

void BHelpBrowser::setCodec(const char *codecName)
{
    setCodec( QTextCodec::codecForName(codecName) );
}
