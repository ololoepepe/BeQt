/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
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

#ifndef BLOCALDOCUMENTDRIVER_H
#define BLOCALDOCUMENTDRIVER_H

class BLocalDocumentDriverPrivate;

class QByteArray;
class QObject;
class QString;
class QStringList;
class QTextCodec;
class QWidget;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQt>
#include <BeQtWidgets/BTextCodecMenu>

/*============================================================================
================================ BLocalDocumentDriver ========================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriver : public BAbstractDocumentDriver
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocalDocumentDriver)
public:
    explicit BLocalDocumentDriver(QObject *parent = 0);
    ~BLocalDocumentDriver();
protected:
    explicit BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent = 0);
public:
    bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec);
    bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec,
                           BeQt::LineFeed &lineFeed);
    QString id() const;
    bool isBuisy() const;
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
    void setCodecsComboBoxStyle(BTextCodecMenu::Style style);
    void setDefaultDir(const QString &dir);
    bool testFileExistance(const QString &fileName);
    bool testFileReadOnly(const QString &fileName);
protected:
    bool handleLoadOperation(const Operation &op);
    bool handleSaveOperation(const Operation &op);
private:
    Q_DISABLE_COPY(BLocalDocumentDriver)
};

#endif // BLOCALDOCUMENTDRIVER_H
