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
class BAbstractCodeEditorDocumentPrivate;
class BTextBlockExtraData;

class QStringList;
class QTextBlock;
class QTextCharFormat;
class QColor;
class QFont;
class QPoint;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QString>
#include <QList>
#include <QIcon>

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
    struct SkipInterval
    {
        int start;
        int end;
    };
    struct AutocompletionItem
    {
        QString text;
        QString toolTip;
        QIcon icon;
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
    static AutocompletionItem createAutocompletionItem(const QString &text, const QString &toolTip = QString(),
                                                       const QIcon &icon = QIcon());
protected:
    virtual void highlightBlock(const QString &text);
    virtual void showAutocompletionMenu(BAbstractCodeEditorDocument *doc, QTextBlock block, int posInBlock,
                                        const QPoint &globalPos);
    virtual QList<AutocompletionItem> createAutocompletionItemList(BAbstractCodeEditorDocument *doc, QTextBlock block,
                                                                   int posInBlock);
    void setBlockSkipIntervals(const QList<SkipInterval> &list = QList<SkipInterval>());
    void clearBlockSkipIntervals();
    void addBlockSkipInterval(const SkipInterval &si);
    void addBlockSkipInterval(int start, int end = -1);
    BAbstractCodeEditorDocument *currentDocument() const;
    QTextBlock currentBlock() const;
    int currentBlockState() const;
    BTextBlockExtraData *currentBlockExtraData() const;
    void setCurrentBlockExtraData(BTextBlockExtraData *data);
    QTextCharFormat format(int position) const;
    int previousBlockState() const;
    void setCurrentBlockState(int newState);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
private:
    friend class BSyntaxHighlighter;
    friend class BAbstractCodeEditorDocumentPrivate;
};

#endif // BABSTRACTFILETYPE_H
