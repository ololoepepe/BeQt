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

#ifndef BABSTRACTCODEEDITORDOCUMENT_P_H
#define BABSTRACTCODEEDITORDOCUMENT_P_H

class BTextBlockUserDataPrivate;

class BCodeEditor;
class BSpellChecker;
class BTextBlockExtraData;

class QColor;
class QEvent;
class QFont;
class QMenu;
class QPlainTextEdit;
class QTextBlock;
class QTextCodec;
class QTextDocument;

#include "babstractcodeeditordocument.h"

#include "babstractdocumentdriver.h"
#include "babstractfiletype.h"

#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/private/bbase_p.h>

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QPair>
#include <QPoint>
#include <QRegExp>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

class BTextBlockUserData : public QTextBlockUserData, public BBase
{
    B_DECLARE_PRIVATE(BTextBlockUserData)
public:
    typedef BAbstractFileType::SkipSegment SkipSegment;
public:
    explicit BTextBlockUserData(const QList<SkipSegment> &list = QList<SkipSegment>());
    ~BTextBlockUserData();
public:
    static bool shouldSkip(const BTextBlockUserData *ud, int pos);
    static bool shouldSkip(const QTextBlock &block, int pos);
    static QList<SkipSegment> skipSegments(const QTextBlock &block);
    static QString textWithoutSkipSegments(const BTextBlockUserData *ud, const QString &text, char replacer = '\0');
    static QString textWithoutSkipSegments(const QTextBlock &block, char replacer = '\0');
public:
    BTextBlockExtraData *extraData() const;
    void setExtraData(BTextBlockExtraData *data);
    void setSkipSegments(const QList<SkipSegment> &list);
    QList<SkipSegment> skipSegments() const;
};

/*============================================================================
================================ BTextBlockUserDataPrivate ===================
============================================================================*/

class BTextBlockUserDataPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTextBlockUserData)
public:
    typedef BAbstractFileType::SkipSegment SkipSegment;
public:
    BTextBlockExtraData *data;
    QList<SkipSegment> skipSegments;
public:
    explicit BTextBlockUserDataPrivate(BTextBlockUserData *q);
    ~BTextBlockUserDataPrivate();
public:
    static bool lessThan(const SkipSegment &s1, const SkipSegment &s2);
    static QList<SkipSegment> processList(const QList<SkipSegment> &list);
public:
    void init();
};

/*============================================================================
================================ BSyntaxHighlighter ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit BSyntaxHighlighter(BAbstractCodeEditorDocument *doc, QTextDocument *parent);
public:
    QTextBlock currentBlock() const;
    BTextBlockExtraData *currentBlockExtraData() const;
    int currentBlockState() const;
    BTextBlockUserData *currentBlockUserData() const;
    BAbstractCodeEditorDocument *editorDocument() const;
    QTextCharFormat format(int position) const;
    int previousBlockState() const;
    void setCurrentBlockExtraData(BTextBlockExtraData *data);
    void setCurrentBlockState(int newState);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
protected:
    void highlightBlock(const QString &text);
public:
    BAbstractCodeEditorDocument *const EditorDocument;
};

/*============================================================================
================================ BAbstractCodeEditorDocumentPrivate ==========
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractCodeEditorDocumentPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractCodeEditorDocument)
public:
    typedef BAbstractFileType::BracketPair BracketPair;
    typedef QList<BracketPair> BracketPairList;
    typedef QTextEdit::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
    typedef BAbstractFileType::SkipSegment SkipSegment;
    typedef BAbstractCodeEditorDocument::TextProcessingResult TextProcessingResult;
    typedef QFuture<TextProcessingResult> TextProcessingResultFuture;
    typedef QFutureWatcher<TextProcessingResult> TextProcessingResultFutureWatcher;
public:
    struct FindBracketPairResult
    {
        const BracketPair *endBr;
        const BracketPair *startBr;
    public:
        int end;
        int start;
    };
    struct SelectionRange
    {
        int end;
        int start;
    };
public:
    typedef QList<FindBracketPairResult> FindBracketPairResultList;
    typedef QList<SelectionRange> SelectionRangeList;
public:
    BCodeEditor *const Editor;
public:
    int asyncMin;
    QTextCursor autocompletionCursor;
    QPoint autocompletionGlobalPos;
    QTimer autocompletionTimer;
    bool bracketsHighlighting;
    bool buisy;
    QTextCodec *codec;
    bool copyAvailable;
    int cursorPosition;
    QPoint cursorPositionRowColumn;
    bool cutAvailable;
    QTextDocument *document;
    QWidget *edit;
    QString fileName;
    BAbstractFileType *fileType;
    bool hasSelection;
    QList<SelectionRange> highlightedBrackets;
    QList<SelectionRange> highlightedBracketsError;
    QList<SelectionRange> highlightedSearchResults;
    BSyntaxHighlighter *highlighter;
    bool isModified;
    QTextDocument::FindFlags lastSearchFlags;
    QRegExp lastSearchRegexp;
    QString lastSearchText;
    bool pasteAvailable;
    bool readOnly;
    BracketPairList recognizedBrackets;
    bool redoAvailable;
    BSpellChecker *spellChecker;
    QTextCursor toolTipCursor;
    QPoint toolTipGlobalPos;
    QTimer toolTipTimer;
    bool undoAvailable;
    BAbstractCodeEditorDocument::ExtraSelectionList userSelections;
    mutable QPair<int, int> wordToReplace;
public:
    explicit BAbstractCodeEditorDocumentPrivate(BAbstractCodeEditorDocument *q, BCodeEditor *editor);
    ~BAbstractCodeEditorDocumentPrivate();
public:
    static void addBlockSkipSegment(QTextBlock block, const BAbstractFileType::SkipSegment &s);
    static void addBlockSkipSegment(QTextBlock block, int start, int end = -1);
    static void addBlockSkipSegmentL(QTextBlock block, int start, int length = -1);
    static QTextCharFormat createBracketsErrorFormat();
    static QTextCharFormat createBracketsFormat();
    static FindBracketPairResult createFindBracketPairResult();
    static QTextCharFormat createSearchResultFormat();
    static int removeExtraSelections(ExtraSelectionList &from, const SelectionRangeList &what);
    static SelectionRangeList removeSelectionRanges(SelectionRangeList &from, int start, int end);
    static bool selectionRangeListsEqual(const SelectionRangeList &list1, const SelectionRangeList &list2);
    static void setBlockSkipSegments(QTextBlock block, const QList<SkipSegment> &list = QList<SkipSegment>());
public:
    bool createEdit();
    QMenu *createSpellCheckerMenu(const QPoint &pos);
    bool eventFilter(QObject *o, QEvent *e);
    FindBracketPairResult findLeftBracketPair() const;
    FindBracketPairResult findRightBracketPair() const;
    void highlightBrackets();
    void init();
    void rehighlight();
    void rehighlightSearchResults();
    void setBuisy(bool b);
    void setCodec(QTextCodec *c);
    void setFileName(QString fn);
    bool testBracket(const QString &text, int posInBlock, bool opening, const BracketPair *&bracket) const;
public Q_SLOTS:
    void customContextMenuRequested(const QPoint &pos);
    void dontIgnoreWord();
    void ignoreWord();
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void preprocessingFinished();
    void replaceWord();
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
    void showAutocompletionMenu();
    void showToolTip();
private:
    Q_DISABLE_COPY(BAbstractCodeEditorDocumentPrivate)
};

#endif // BABSTRACTCODEEDITORDOCUMENT_P_H

