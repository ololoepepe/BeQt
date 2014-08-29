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

class BAbstractFileType;

#include "blocaldocumentdriver.h"
#include "blocaldocumentdriver_p.h"

#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"
#include "bcodeeditor.h"
#include "bextendedfiledialog.h"

#include <BeQtCore/BBase>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BDirTools>
#include <BeQtCore/BeQt>
#include <BeQtCore/BTextTools>
#include <BeQtCore/private/bbase_p.h>

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtConcurrentRun>
#include <QTextCodec>
#include <QTextStream>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalDocumentDriverPrivate::BLocalDocumentDriverPrivate(BLocalDocumentDriver *q) :
    BAbstractDocumentDriverPrivate(q)
{
    //
}

BLocalDocumentDriverPrivate::~BLocalDocumentDriverPrivate()
{
    //
}

/*============================== Static public methods =====================*/

BLocalDocumentDriverPrivate::LoadResult BLocalDocumentDriverPrivate::loadFile(const Operation &op, const QString &fn,
                                                                              QTextCodec *codec, bool autoDetection)
{
    QFile f(fn);
    LoadResult res;
    res.operation = op;
    bool ok = false;
    QByteArray data = BDirTools::readFile(fn, -1, &ok);
    if (!ok) {
        res.success = false;
        return res;
    }
    QTextCodec *c = 0;
    if (autoDetection)
        c = BTextTools::guessTextCodec(data, BCoreApplication::locale());
    if (!c)
        c = codec;
    else
        res.operation.codec = c;
    QTextStream in(data);
    in.setCodec(c);
    res.text = in.readAll();
    f.close();
    res.success = true;
    return res;
}

BLocalDocumentDriverPrivate::SaveResult BLocalDocumentDriverPrivate::saveFile(const Operation &op, const QString &fn,
                                                                              QString txt, QTextCodec *codec,
                                                                              BeQt::LineFeed lineFeed)
{
    QFile f(fn);
    SaveResult res;
    res.operation = op;
    if (!f.open(QFile::WriteOnly)) {
        res.success = false;
        return res;
    }
    QTextStream out(&f);
    out.setCodec(codec);
    if (BeQt::DefaultLineFeed == lineFeed)
        lineFeed = BeQt::platformLineFeed();
    switch (lineFeed) {
    case BeQt::ClassicMacLineFeed:
        txt.replace('\n', '\r');
        break;
    case BeQt::WindowsLineFeed:
        txt.replace('\n', "\r\n");
        break;
    case BeQt::UnixLineFeed:
    default:
        break;
    }
    out << txt;
    f.close();
    res.success = true;
    return res;
}

/*============================== Public methods ============================*/

void BLocalDocumentDriverPrivate::init()
{
    defaultDir = QDir::homePath();
    lastFileType = 0;
    codecsComboBoxStyle = BTextCodecMenu::StructuredStyle;
}

/*============================== Public slots ==============================*/

void BLocalDocumentDriverPrivate::fixFileNameEncoding(QString &fileName)
{
    QByteArray data = fileName.toLocal8Bit();
    fileName = QTextCodec::codecForLocale()->toUnicode(data);
}

void BLocalDocumentDriverPrivate::loadOperationFinished()
{
    LoadResultFutureWatcher *watcher = dynamic_cast<LoadResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    LoadResult res = watcher->result();
    delete watcher;
    q_func()->emitLoadingFinished(res.operation, res.success, res.text);
}

void BLocalDocumentDriverPrivate::saveOperationFinished()
{
    SaveResultFutureWatcher *watcher = dynamic_cast<SaveResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    SaveResult res = watcher->result();
    delete watcher;
    q_func()->emitSavingFinished(res.operation, res.success);
}

/*============================================================================
================================ BLocalDocumentDriver ========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalDocumentDriver::BLocalDocumentDriver(QObject *parent) :
    BAbstractDocumentDriver(*new BLocalDocumentDriverPrivate(this), parent)
{
    d_func()->init();
}

BLocalDocumentDriver::~BLocalDocumentDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BLocalDocumentDriver::BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent) :
    BAbstractDocumentDriver(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

BTextCodecMenu::Style BLocalDocumentDriver::codecsComboBoxStyle() const
{
    return d_func()->codecsComboBoxStyle;
}

QString BLocalDocumentDriver::defaultDir() const
{
    return d_func()->defaultDir;
}

bool BLocalDocumentDriver::getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec)
{
    if (!editor())
        return false;
    B_D(BLocalDocumentDriver);
    BExtendedFileDialog bfd(d->codecsComboBoxStyle, parent);
    bfd.restoreState(d->fileDialogState);
    if (!bfd.restoreGeometry(d->fileDialogGeometry))
        bfd.resize(700, 400);
    if (d->fileDialogState.isEmpty())
        bfd.setDirectory(d->defaultDir);
    bfd.setFileTypes(editor()->fileTypes());
    bfd.selectFileType(d->lastFileType ? d->lastFileType : editor()->preferredFileType());
    if (codec)
        bfd.selectCodec(codec);
    bfd.setAcceptMode(BExtendedFileDialog::AcceptOpen);
    bfd.setFileMode(QFileDialog::ExistingFiles);
    int ret = bfd.exec();
    d->fileDialogState = bfd.saveState();
    d->fileDialogGeometry = bfd.saveGeometry();
    d->lastFileType = bfd.selectedFileType();
    if (BExtendedFileDialog::Accepted != ret)
        return false;
    fileNames = bfd.selectedFiles();
    foreach (int i, bRangeD(0, fileNames.size() - 1))
        BLocalDocumentDriverPrivate::fixFileNameEncoding(fileNames[i]);
    codec = bfd.selectedCodec();
    return true;
}

bool BLocalDocumentDriver::getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName,
                                             QTextCodec *&codec, BeQt::LineFeed &lineFeed)
{
    if (!editor())
        return false;
    B_D(BLocalDocumentDriver);
    BExtendedFileDialog bfd(d->codecsComboBoxStyle, parent);
    bfd.restoreState(d->fileDialogState);
    if (!bfd.restoreGeometry(d->fileDialogGeometry))
        bfd.resize(700, 400);
    bfd.setFileTypes(editor()->fileTypes());
    bfd.selectCodec(codec);
    bfd.setAcceptMode(BExtendedFileDialog::AcceptSave);
    bfd.setFileMode(QFileDialog::AnyFile);
    QString dir = QFileInfo(fileName).path();
    if (dir != "." && QDir(dir).exists())
        bfd.setDirectory(dir); //TODO: Maybe should improve (needs testing)
    else if (d->fileDialogState.isEmpty())
        bfd.setDirectory(d->defaultDir);
    bfd.selectFile(fileName);
    //bfd.selectLineFeed(lineFeed);
    int ret = bfd.exec();
    d->fileDialogState = bfd.saveState();
    d->fileDialogGeometry = bfd.saveGeometry();
    if (BExtendedFileDialog::Accepted != ret)
        return false;
    newName = bfd.selectedFiles().first();
    BLocalDocumentDriverPrivate::fixFileNameEncoding(newName);
    codec = bfd.selectedCodec();
    lineFeed = bfd.selectedLineFeed();
    return true;
}

QString BLocalDocumentDriver::id() const
{
    return "beqt/local";
}

bool BLocalDocumentDriver::isBuisy() const
{
    return false;
}

void BLocalDocumentDriver::restoreState(const QByteArray &state)
{
    QVariantMap m = BeQt::deserialize(state).toMap();
    d_func()->fileDialogState = m.value("file_dialog_state").toByteArray();
    d_func()->fileDialogGeometry = m.value("file_dialog_geometry").toByteArray();
}

QByteArray BLocalDocumentDriver::saveState() const
{
    QVariantMap m;
    m.insert("file_dialog_state", d_func()->fileDialogState);
    m.insert("file_dialog_geometry", d_func()->fileDialogGeometry);
    return BeQt::serialize(m);
}

void BLocalDocumentDriver::setCodecsComboBoxStyle(BTextCodecMenu::Style style)
{
    d_func()->codecsComboBoxStyle = style;
}

void BLocalDocumentDriver::setDefaultDir(const QString &dir)
{
    if (dir.isEmpty())
        return;
    d_func()->defaultDir = dir;
}

bool BLocalDocumentDriver::testFileExistance(const QString &fileName)
{
    return QFileInfo(fileName).isFile();
}

bool BLocalDocumentDriver::testFileReadOnly(const QString &fileName)
{
    QFile f(fileName);
    bool b = !f.open(QFile::WriteOnly | QFile::Append);
    f.close();
    return b;
}

/*============================== Protected methods =========================*/

bool BLocalDocumentDriver::handleLoadOperation(const Operation &op)
{
    QString fn = !op.fileName.isEmpty() ? op.fileName : op.document->fileName();
    QTextCodec *c = op.codec ? op.codec : op.document->codec();
    BLocalDocumentDriverPrivate::LoadResultFuture future =
            QtConcurrent::run(&BLocalDocumentDriverPrivate::loadFile, op, fn, c,
                              editor()->isAutoCodecDetectionEnabled());
    BLocalDocumentDriverPrivate::LoadResultFutureWatcher *watcher =
            new BLocalDocumentDriverPrivate::LoadResultFutureWatcher(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), d_func(), SLOT(loadOperationFinished()));
    return true;
}

bool BLocalDocumentDriver::handleSaveOperation(const Operation &op)
{
    QString fn = !op.fileName.isEmpty() ? op.fileName : op.document->fileName();
    QTextCodec *c = op.codec ? op.codec : op.document->codec();
    QString txt = op.document->text();
    BLocalDocumentDriverPrivate::SaveResultFuture future =
            QtConcurrent::run(&BLocalDocumentDriverPrivate::saveFile, op, fn, txt, c, op.lineFeed);
    BLocalDocumentDriverPrivate::SaveResultFutureWatcher *watcher =
            new BLocalDocumentDriverPrivate::SaveResultFutureWatcher(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), d_func(), SLOT(saveOperationFinished()));
    return true;
}
