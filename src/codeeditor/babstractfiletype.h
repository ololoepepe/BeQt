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

class BAbstractFileTypePrivate;

class BAbstractCodeEditorDocument;
class BAbstractCodeEditorDocumentPrivate;
class BSyntaxHighlighter;
class BTextBlockExtraData;

class QColor;
class QFont;
class QPoint;
class QStringList;
class QTextBlock;
class QTextCharFormat;
class QTextCursor;

#include <BeQtCore/BBase>

#include <QIcon>
#include <QList>
#include <QString>

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileType : public BBase
{
    B_DECLARE_PRIVATE(BAbstractFileType)
public:
    struct AutocompletionItem
    {
        QIcon actionIcon;
        QString actionText;
        QString actionToolTip;
        QString text;
    };
    struct BracketPair
    {
        QString closing;
        QString escape;
        QString opening;
    };
    struct SkipSegment
    {
        int end;
        int start;
    };
public:
    typedef QList<BracketPair> BracketPairList;
public:
    explicit BAbstractFileType();
    virtual ~BAbstractFileType();
public:
    static bool areEqual(const BracketPair &bp1, const BracketPair &bp2);
    static bool areEqual(const BracketPairList &l1, const BracketPairList &l2);
    static BAbstractFileType *defaultFileType();
    static QString defaultFileTypeId();
public:
    virtual bool autocompletionMenuVisible(BAbstractCodeEditorDocument *doc) const;
    virtual BracketPairList brackets() const = 0;
    QString createFileDialogFilter() const;
    virtual QString description() const = 0;
    virtual QString id() const = 0;
    virtual int indentation(const QTextBlock &previousBlock) const;
    virtual bool matchesFileName(const QString &fileName) const;
    virtual QString name() const = 0;
    virtual QStringList suffixes() const = 0;
protected:
    static AutocompletionItem createAutocompletionItem(const QString &text, const QString &actionText,
                                                       const QString &actionToolTip = QString(),
                                                       const QIcon &actionIcon = QIcon());
    static BracketPair createBracketPair(const QString &op, const QString &cl, const QString &esc = QString());
protected:
    void addCurrentBlockSkipSegment(const SkipSegment &s);
    void addCurrentBlockSkipSegment(int start, int end = -1);
    void addCurrentBlockSkipSegmentL(int start, int length = -1);
    void clearCurrentBlockSkipSegments();
    virtual QList<AutocompletionItem> createAutocompletionItemList(BAbstractCodeEditorDocument *doc,
                                                                   QTextCursor cursor);
    virtual QString createToolTipText(BAbstractCodeEditorDocument *doc, QTextCursor cursor);
    QTextBlock currentBlock() const;
    BTextBlockExtraData *currentBlockExtraData() const;
    int currentBlockState() const;
    BAbstractCodeEditorDocument *currentDocument() const;
    QTextCharFormat format(int position) const;
    virtual void highlightBlock(const QString &text);
    int previousBlockState() const;
    void setCurrentBlockExtraData(BTextBlockExtraData *data);
    void setCurrentBlockSkipSegments(const QList<SkipSegment> &list = QList<SkipSegment>());
    void setCurrentBlockState(int newState);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
    virtual void showAutocompletionMenu(BAbstractCodeEditorDocument *doc, QTextCursor cursor, const QPoint &globalPos);
    virtual void showToolTip(BAbstractCodeEditorDocument *doc, QTextCursor cursor, const QPoint &globalPos);
private:
    friend class BSyntaxHighlighter;
    friend class BAbstractCodeEditorDocumentPrivate;
};

#endif // BABSTRACTFILETYPE_H
