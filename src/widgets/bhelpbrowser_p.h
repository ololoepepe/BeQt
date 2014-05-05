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
class QPushButton;

#include "bhelpbrowser.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QStringList>
#include <QObject>
#include <QMap>
#include <QPair>

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
public Q_SLOTS:
    void retranslateUi();
    void updateCaption();
    void search();
public:
    static QMap< QString, QPair<QStringList, QStringList> > searchCache;
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
        QPushButton *btnFind;
      QTextBrowser *tbrsr;
private:
    Q_DISABLE_COPY(BHelpBrowserPrivate)
};

#endif // BHELPBROWSER_P_H
