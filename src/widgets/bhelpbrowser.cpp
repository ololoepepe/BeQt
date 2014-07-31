/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bhelpbrowser.h"
#include "bhelpbrowser_p.h"

#include "bapplication.h"

#include <BeQtCore/BTextTools>
#include <BeQtCore/BDirTools>
#include <BeQtCore/BTextMatch>
#include <BeQtCore/BTextMatchList>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPair>
#include <QPushButton>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextBrowser>
#include <QTextCodec>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ BHelpBrowserPrivate =========================
============================================================================*/

/*============================== Static public variables ===================*/

QMap< QString, QPair<QStringList, QStringList> > BHelpBrowserPrivate::searchCache;

/*============================== Public constructors =======================*/

BHelpBrowserPrivate::BHelpBrowserPrivate(BHelpBrowser *q) :
    BBaseObjectPrivate(q)
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
      QToolBar *tbar = new QToolBar(q);
        tbtnBackward = new QToolButton;
          tbtnBackward->setEnabled(false);
          tbtnBackward->setIcon(BApplication::icon("back"));
        tbar->addWidget(tbtnBackward);
        tbtnForward = new QToolButton;
          tbtnForward->setEnabled(false);
          tbtnForward->setIcon(BApplication::icon("forward"));
        tbar->addWidget(tbtnForward);
        tbar->addSeparator();
        tbtnHome = new QToolButton;
          tbtnHome->setIcon(BApplication::icon("gohome"));
        tbar->addWidget(tbtnHome);
        tbar->addSeparator();
        lblSearch = new QLabel;
        tbar->addWidget(lblSearch);
        ledtSearch = new QLineEdit;
          connect(ledtSearch, SIGNAL(returnPressed()), this, SLOT(search()));
        tbar->addWidget(ledtSearch);
        btnFind = new QPushButton;
          connect(btnFind, SIGNAL(clicked()), this, SLOT(search()));
          connect(btnFind, SIGNAL(clicked()), ledtSearch, SLOT(setFocus()));
        tbar->addWidget(btnFind);
      vlt->addWidget(tbar);
      tbrsr = new QTextBrowser(q);
        connect(tbtnBackward, SIGNAL(clicked()), tbrsr, SLOT(backward()));
        connect(tbtnForward, SIGNAL(clicked()), tbrsr, SLOT(forward()));
        connect(tbtnHome, SIGNAL(clicked()), tbrsr, SLOT(home()));
        connect(tbrsr, SIGNAL(backwardAvailable(bool)), tbtnBackward, SLOT(setEnabled(bool)));
        connect(tbrsr, SIGNAL(forwardAvailable(bool)), tbtnForward, SLOT(setEnabled(bool)));
        connect(tbrsr, SIGNAL(sourceChanged(QUrl)), this, SLOT(updateCaption()));
        tbrsr->setOpenExternalLinks(true);
      vlt->addWidget(tbrsr);
    //
    QTimer::singleShot(0, ledtSearch, SLOT(setFocus()));
    retranslateUi();
    QObject::connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
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

void BHelpBrowserPrivate::search()
{
    QString text = ledtSearch->text();
    if (text.isEmpty())
        return;
    QPair<QStringList, QStringList> p;
    QStringList &sl = p.first;
    QStringList &documents = p.second;
    if (!searchCache.contains(text)) {
        QStringList terms = BTextTools::splitCommand(text);
        foreach (const QString &path, tbrsr->searchPaths()) {
            QStringList files = BDirTools::entryList(path, QStringList() << "*.html" << "*.htm", QDir::Files);
            foreach (const QString &file, files)  {
                sl << file;
                documents << BDirTools::readTextFile(file, codec);
            }
        }
        QList<int> list = BTextTools::tfidfSortedIndexes(terms, documents);
        foreach (int i, bRangeD(0, list.size() - 1)) {
            sl.swap(i, list.at(i));
            documents.swap(i, list.at(i));
        }
        sl = sl.mid(0, list.size());
        documents = documents.mid(0, list.size());
        searchCache.insert(text, qMakePair(sl, documents));
    } else {
        p = searchCache.value(text);
    }
    QString source = "<center><font size=5><b>" + tr("Search results", "tbrsr text") + "</b></font></center><br><br>";
    if (!sl.isEmpty()) {
        foreach (int i, bRangeD(0, sl.size() - 1)) {
            BTextMatchList ml = BTextTools::match(documents.at(i), QRegExp(".+"),
                                                  QRegExp("<title>(\n)*", Qt::CaseInsensitive),
                                                  QRegExp("(\n)*</title>", Qt::CaseInsensitive));
            if (ml.isEmpty())
                continue;
            source += "<a href=\"" + QFileInfo(sl.at(i)).fileName() + "\">" + ml.first() + "</a><br>";
        }
    } else {
        source += "<img src=\"" + BDirTools::findResource("beqt/pixmaps/sadpanda.jpg", BDirTools::GlobalOnly) + "\">";
        source += "<br><br>" + tr("Sorry, nothing found.", "tbrsr text");
    }
    source += "<br><br><a href=\"" + tbrsr->source().toString() + "\">" + tr("Back", "tbrsr text") + "</a>";
    tbrsr->setHtml(source);
}

void BHelpBrowserPrivate::updateCaption()
{
    QString title = tr("Help", "windowTitle");
    QString dt = tbrsr->documentTitle();
    if (!dt.isEmpty())
        title += ": " + dt;
    q_func()->setWindowTitle(title);
    QTimer::singleShot(0, ledtSearch, SLOT(setFocus()));
}

/*============================================================================
================================ BHelpBrowser ================================
============================================================================*/

/*============================== Public constructors =======================*/

BHelpBrowser::BHelpBrowser(QWidget *parent) :
    QWidget(parent), BBaseObject(*new BHelpBrowserPrivate(this))
{
    d_func()->init();
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BHelpBrowserPrivate(this))
{
    d_func()->init();
    d_func()->tbrsr->setSearchPaths(searchPaths);
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &file, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BHelpBrowserPrivate(this))
{
    d_func()->init();
    B_D(BHelpBrowser);
    d->tbrsr->setSearchPaths(searchPaths);
    d->tbrsr->setSource(QUrl(file));
}

BHelpBrowser::BHelpBrowser(const QStringList &searchPaths, const QString &index, const QString &file,
                           QWidget *parent) :
    QWidget(parent), BBaseObject(*new BHelpBrowserPrivate(this))
{
    d_func()->init();
    B_D(BHelpBrowser);
    d->tbrsr->setSearchPaths(searchPaths);
    if (!index.isEmpty())
        d->tbrsr->setSource(QUrl(index));
    if (file != index)
        d->tbrsr->setSource(QUrl(file));
}

BHelpBrowser::~BHelpBrowser()
{
    //
}

/*============================== Protected constructors ====================*/

BHelpBrowser::BHelpBrowser(BHelpBrowserPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

void BHelpBrowser::clearSearchCache()
{
    BHelpBrowserPrivate::searchCache.clear();
}

/*============================== Public methods ============================*/

QTextCodec *BHelpBrowser::codec() const
{
    return d_func()->codec;
}

QString BHelpBrowser::codecName() const
{
    return d_func()->codec ? QString::fromLatin1(d_func()->codec->name()) : QString();
}

QString BHelpBrowser::fileName() const
{
    return d_func()->tbrsr->source().path();
}

QStringList BHelpBrowser::searchPaths() const
{
    return d_func()->tbrsr->searchPaths();
}

void BHelpBrowser::setCodec(QTextCodec *codec)
{
    if (!codec)
        return;
    d_func()->codec = codec;
}

void BHelpBrowser::setCodec(const QString &codecName)
{
    setCodec(QTextCodec::codecForName(codecName.toLatin1()));
}

void BHelpBrowser::setFile(const QString &file)
{
    d_func()->tbrsr->setSource(QUrl(file));
}

void BHelpBrowser::setSearchPaths(const QStringList &paths)
{
    d_func()->tbrsr->setSearchPaths(paths);
}
