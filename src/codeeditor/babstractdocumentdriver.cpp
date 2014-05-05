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

class BCodeEditor;

class QTextCodec;

#include "babstractdocumentdriver.h"
#include "babstractdocumentdriver_p.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QByteArray>

#include <QDebug>

/*============================================================================
================================ BAbstractDocumentDriverPrivate ==============
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractDocumentDriverPrivate::BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q) :
    BBasePrivate(q)
{
    //
}

BAbstractDocumentDriverPrivate::~BAbstractDocumentDriverPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractDocumentDriverPrivate::init()
{
    editor = 0;
}

void BAbstractDocumentDriverPrivate::setEditor(BCodeEditor *edr)
{
    editor = edr;
}

/*============================================================================
================================ BAbstractDocumentDriver =====================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(QObject *parent) :
    QObject(parent)
{
    d_func()->init();
}

BAbstractDocumentDriver::~BAbstractDocumentDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QByteArray BAbstractDocumentDriver::saveState() const
{
    return QByteArray();
}

void BAbstractDocumentDriver::restoreState(const QByteArray &)
{
    //
}

bool BAbstractDocumentDriver::load(BAbstractCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if (!doc)
        return false;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    return handleLoadOperation(op);
}

bool BAbstractDocumentDriver::save(BAbstractCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if (!doc)
        return false;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    return handleSaveOperation(op);
}

bool BAbstractDocumentDriver::load(BAbstractCodeEditorDocument *doc, const QString &fileName)
{
    return load(doc, 0, fileName);
}

bool BAbstractDocumentDriver::save(BAbstractCodeEditorDocument *doc, const QString &fileName)
{
    return save(doc, 0, fileName);
}

BCodeEditor *BAbstractDocumentDriver::editor() const
{
    return d_func()->editor;
}

/*============================== Protected methods =========================*/

void BAbstractDocumentDriver::emitLoadingFinished(const Operation &operation, bool success, const QString &text)
{
    Q_EMIT loadingFinished(operation, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(const Operation &operation, bool success)
{
    Q_EMIT savingFinished(operation, success);
}
