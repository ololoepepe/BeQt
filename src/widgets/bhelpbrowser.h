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

#ifndef BHELPBROWSER_H
#define BHELPBROWSER_H

class BHelpBrowserPrivate;

class QString;
class QStringList;
class QTextCodec;

#include <BeQtCore/BBaseObject>

#include <QWidget>

/*============================================================================
================================ BHelpBrowser ================================
============================================================================*/

class B_WIDGETS_EXPORT BHelpBrowser : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QTextCodec * codec READ codec WRITE setCodec)
    Q_PROPERTY(QString codecName READ codecName WRITE setCodec)
    Q_PROPERTY(QString fileName READ fileName WRITE setFile)
    Q_PROPERTY(QStringList searchPaths READ searchPaths WRITE setSearchPaths)
    B_DECLARE_PRIVATE(BHelpBrowser)
public:
    typedef bool (*UrlHandlerFunction)(const QUrl &url);
public:
    explicit BHelpBrowser(QWidget *parent = 0);
    explicit BHelpBrowser(const QStringList &searchPaths, QWidget *parent = 0);
    explicit BHelpBrowser(const QStringList &searchPaths, const QString &file, QWidget *parent = 0);
    explicit BHelpBrowser(const QStringList &searchPaths, const QString &index, const QString &file,
                          QWidget *parent = 0);
    ~BHelpBrowser();
protected:
    explicit BHelpBrowser(BHelpBrowserPrivate &d, QWidget *parent = 0);
public:
    static void clearSearchCache();
    static void setUserUrlHandlerFunction(UrlHandlerFunction f);
    static UrlHandlerFunction userUrlHandlerFunction();
public:
    QTextCodec *codec() const;
    QString codecName() const;
    QString fileName() const;
    QStringList searchPaths() const;
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    void setFile(const QString &fileName);
    void setSearchPaths(const QStringList &paths);
private:
    Q_DISABLE_COPY(BHelpBrowser)
};

#endif // BHELPBROWSER_H
