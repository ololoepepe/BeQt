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

#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class BSyntaxHighlighter;
class BAbstractFileTypePrivate;
class BAbstractCodeEditorDocument;

class QStringList;
class QTextBlock;
class QTextCharFormat;
class QColor;
class QFont;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include "btextblockuserdata.h"

#include <QString>
#include <QList>

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileType : public BBase
{
    B_DECLARE_PRIVATE(BAbstractFileType)
public:
    struct BracketPair
    {
        QString opening;
        QString closing;
        QString escape;
    };
public:
    typedef QList<BracketPair> BracketPairList;
public:
    explicit BAbstractFileType();
    virtual ~BAbstractFileType();
public:
    static BAbstractFileType *defaultFileType();
    static QString defaultFileTypeId();
    static bool areEqual(const BracketPair &bp1, const BracketPair &bp2);
    static bool areEqual(const BracketPairList &l1, const BracketPairList &l2);
public:
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
    virtual bool matchesFileName(const QString &fileName) const = 0;
    virtual BracketPairList brackets() const = 0;
    virtual int indentation(const QTextBlock &previousBlock) const;
    QString createFileDialogFilter() const;
protected:
    static BracketPair createBracketPair(const QString &op, const QString &cl, const QString &esc = QString());
    static void setBlockSkipIntervals(QTextBlock block, const QList<BTextBlockUserData::SkipInterval> &list
                                      = QList<BTextBlockUserData::SkipInterval>());
    static void addBlockSkipInterval(QTextBlock block, const BTextBlockUserData::SkipInterval &si);
    static void addBlockSkipInterval(QTextBlock block, int start, int end = -1);
protected:
    virtual void highlightBlock(const QString &text) = 0;
    QTextBlock currentBlock() const;
    int currentBlockState() const;
    BTextBlockUserData *currentBlockUserData() const;
    QTextCharFormat format(int position) const;
    int previousBlockState() const;
    void setCurrentBlockState(int newState);
    void setCurrentBlockUserData(BTextBlockUserData *data);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
private:
    friend class BSyntaxHighlighter;
};

#endif // BABSTRACTFILETYPE_H
