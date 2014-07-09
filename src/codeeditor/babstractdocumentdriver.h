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

#ifndef BABSTRACTDOCUMENTDRIVER_H
#define BABSTRACTDOCUMENTDRIVER_H

class BAbstractDocumentDriverPrivate;

class BAbstractCodeEditorDocument;
class BCodeEditor;
class BCodeEditorPrivate;

class QByteArray;
class QTextCodec;
class QWidget;

#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QString>

/*============================================================================
================================ BAbstractDocumentDriver =====================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractDocumentDriver : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAbstractDocumentDriver)
public:
    struct Operation
    {
        QTextCodec *codec;
        const BAbstractCodeEditorDocument *document;
        QString fileName;
    };
public:
    explicit BAbstractDocumentDriver(QObject *parent = 0);
    ~BAbstractDocumentDriver();
protected:
    explicit BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent = 0);
public:
    BCodeEditor *editor() const;
    virtual bool getOpenFileNames(QWidget *parent, QStringList &fileNames, QTextCodec *&codec) = 0;
    virtual bool getSaveAsFileName(QWidget *parent, const QString &fileName, QString &newName, QTextCodec *&codec) = 0;
    virtual QString id() const = 0;
    virtual bool isBuisy() const = 0;
    bool load(BAbstractCodeEditorDocument *doc, const QString &fileName = QString());
    bool load(BAbstractCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString());
    virtual void restoreState(const QByteArray &state);
    bool save(BAbstractCodeEditorDocument *doc, const QString &fileName = QString());
    bool save(BAbstractCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName = QString());
    virtual QByteArray saveState() const;
    virtual bool testFileExistance(const QString &fileName) = 0;
    virtual bool testFileReadOnly(const QString &fileName) = 0;
protected:
    void emitLoadingFinished(const Operation &operation, bool success, const QString &text = QString());
    void emitSavingFinished(const Operation &operation, bool success);
    virtual bool handleLoadOperation(const Operation &op) = 0;
    virtual bool handleSaveOperation(const Operation &op) = 0;
Q_SIGNALS:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
private:
    Q_DISABLE_COPY(BAbstractDocumentDriver)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTDOCUMENTDRIVER_H
