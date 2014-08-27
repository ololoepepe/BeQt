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

#ifndef BABSTRACTFILETYPE_P_H
#define BABSTRACTFILETYPE_P_H

class BAbstractCodeEditorDocument;
class BSyntaxHighlighter;

class QEvent;
class QModelIndex;
class QStandardItemModel;
class QString;
class QStringList;

#include "babstractfiletype.h"

#include <BeQtCore/private/bbase_p.h>

#include <QCoreApplication>
#include <QObject>
#include <QTextCursor>

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

class BDefaultFileType : public BAbstractFileType
{
    Q_DECLARE_TR_FUNCTIONS(BDefaultFileType)
protected:
    static BDefaultFileType *_m_self;
public:
    explicit BDefaultFileType();
    ~BDefaultFileType();
public:
    static BDefaultFileType *instance();
public:
    BracketPairList brackets() const;
    QString description() const;
    QString id() const;
    bool matchesFileName(const QString &fileName) const;
    QString name() const;
    QStringList suffixes() const;
};

/*============================================================================
================================ BAutoCompletionHelper =======================
============================================================================*/

class B_CODEEDITOR_EXPORT BAutoCompletionHelper : public QObject
{
    Q_OBJECT
public:
    BAbstractCodeEditorDocument * const Doc;
    QStandardItemModel * const Model;
public:
    QTextCursor textCursor;
public:
    explicit BAutoCompletionHelper(BAbstractCodeEditorDocument *parent, QStandardItemModel *model);
public:
    bool eventFilter(QObject *object, QEvent *event);
public Q_SLOTS:
    void completerActivated(const QModelIndex &index);
};

/*============================================================================
================================ BAbstractFileTypePrivate ====================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileTypePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractFileType)
public:
    BSyntaxHighlighter *highlighter;
public:
    explicit BAbstractFileTypePrivate(BAbstractFileType *q);
    ~BAbstractFileTypePrivate();
public:
    void init();
    void setDocument(BAbstractCodeEditorDocument *doc);
private:
    Q_DISABLE_COPY(BAbstractFileTypePrivate)
};

#endif // BABSTRACTFILETYPE_P_H
