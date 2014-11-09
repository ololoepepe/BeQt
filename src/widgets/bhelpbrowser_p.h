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

class QLabel;
class QLineEdit;
class QPushButton;
class QTextBrowser;
class QTextCodec;
class QToolButton;
class QUrl;

#include "bhelpbrowser.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BHelpBrowserPrivate =========================
============================================================================*/

class B_WIDGETS_EXPORT BHelpBrowserPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BHelpBrowser)
public:
    static QMap< QString, QPair<QStringList, QStringList> > searchCache;
    static BHelpBrowser::UrlHandlerFunction urlHandlerFunction;
public:
    QPushButton *btnFind;
    QTextCodec *codec;
    QLabel *lblSearch;
    QLineEdit *ledtSearch;
    QTextBrowser *tbrsr;
    QToolButton *tbtnBackward;
    QToolButton *tbtnForward;
    QToolButton *tbtnHome;
public:
    explicit BHelpBrowserPrivate(BHelpBrowser *q);
    ~BHelpBrowserPrivate();
public:
    void init();
public Q_SLOTS:
    void anchorClicked(const QUrl &url);
    void retranslateUi();
    void search();
    void updateCaption();
private:
    Q_DISABLE_COPY(BHelpBrowserPrivate)
};

#endif // BHELPBROWSER_P_H
