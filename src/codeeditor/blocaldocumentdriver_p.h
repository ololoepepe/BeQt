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

#ifndef BLOCALDOCUMENTDRIVER_P_H
#define BLOCALDOCUMENTDRIVER_P_H

class BAbstractFileType;

class QTextCodec;

#include "blocaldocumentdriver.h"

#include "babstractdocumentdriver_p.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QByteArray>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QString>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BAbstractDocumentDriverPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
public:
    typedef BLocalDocumentDriver::Operation Operation;
public:
    struct LoadResult
    {
        Operation operation;
        bool success;
        QString text;
    };
    struct SaveResult
    {
        Operation operation;
        bool success;
    };
public:
    typedef QFuture<LoadResult> LoadResultFuture;
    typedef QFutureWatcher<LoadResult> LoadResultFutureWatcher;
    typedef QFuture<SaveResult> SaveResultFuture;
    typedef QFutureWatcher<SaveResult> SaveResultFutureWatcher;
public:
    BTextCodecMenu::Style codecsComboBoxStyle;
    QString defaultDir;
    QByteArray fileDialogGeometry;
    QByteArray fileDialogState;
    BAbstractFileType *lastFileType;
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
public:
    static void fixFileNameEncoding(QString &fileName);
    static LoadResult loadFile(const Operation &op, const QString &fn, QTextCodec *codec, bool autoDetection);
    static SaveResult saveFile(const Operation &op, const QString &fn, QString txt, QTextCodec *codec,
                               BeQt::LineFeed lineFeed);
public:
    void init();
public Q_SLOTS:
    void loadOperationFinished();
    void saveOperationFinished();
private:
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
};

#endif // BLOCALDOCUMENTDRIVER_P_H
