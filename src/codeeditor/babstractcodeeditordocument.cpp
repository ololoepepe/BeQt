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

#include "babstractcodeeditordocument.h"
#include "babstractcodeeditordocument_p.h"

#include "babstractdocumentdriver.h"
#include "babstractfiletype.h"
#include "babstractfiletype_p.h"
#include "bcodeedit.h"
#include "bcodeeditor.h"
#include "btextblockextradata.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/BSpellChecker>
#include <BeQtCore/BTextTools>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtWidgets/BPlainTextEdit>

#include <QAction>
#include <QBrush>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFont>
#include <QFuture>
#include <QFutureWatcher>
#include <QHelpEvent>
#include <QLayout>
#include <QList>
#include <QMenu>
#include <QMetaObject>
#include <QObject>
#include <QPair>
#include <QPlainTextEdit>
#include <QPoint>
#include <QRegExp>
#include <QString>
#include <QSyntaxHighlighter>
#include <QtAlgorithms>
#include <QtConcurrentRun>
#include <QTextBlockUserData>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ BTextBlockUserDataPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserDataPrivate::BTextBlockUserDataPrivate(BTextBlockUserData *q) :
    BBasePrivate(q)
{
    //
}

BTextBlockUserDataPrivate::~BTextBlockUserDataPrivate()
{
    delete data;
}

/*============================== Static public methods =====================*/

bool BTextBlockUserDataPrivate::lessThan(const SkipSegment &s1, const SkipSegment &s2)
{
    return s1.start < s2.start;
}

QList<BTextBlockUserDataPrivate::SkipSegment> BTextBlockUserDataPrivate::processList(const QList<SkipSegment> &list)
{
    if (list.isEmpty())
        return list;
    QList<BAbstractFileType::SkipSegment> nlist = list;
    foreach (int i, bRangeR(nlist.size() - 1, 0)) {
        const SkipSegment &s = nlist.at(i);
        if (s.start < 0 || s.end < 0 || s.start > s.end)
            nlist.removeAt(i);
    }
    if (nlist.isEmpty())
        return nlist;
    qSort(nlist.begin(), nlist.end(), &lessThan);
    foreach (int i, bRangeR(nlist.size() - 1, 1)) {
        SkipSegment &s1 = nlist[i - 1];
        const SkipSegment &s2 = nlist.at(i);
        if (s1.end >= s2.start) {
            s1.end = s2.end;
            nlist.removeAt(i);
        }
    }
    return nlist;
}

/*============================== Public methods ============================*/

void BTextBlockUserDataPrivate::init()
{
    data = 0;
}

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserData::BTextBlockUserData(const QList<SkipSegment> &list) :
    BBase(*new BTextBlockUserDataPrivate(this))
{
    d_func()->init();
    setSkipSegments(list);
}

BTextBlockUserData::~BTextBlockUserData()
{
    //
}

/*============================== Static public methods =====================*/

bool BTextBlockUserData::shouldSkip(const BTextBlockUserData *ud, int pos)
{
    if (!ud || pos < 0)
        return false;
    foreach (const SkipSegment &s, ud->d_func()->skipSegments) {
        if (s.start <= pos && s.end >= pos)
            return true;
    }
    return false;
}

bool BTextBlockUserData::shouldSkip(const QTextBlock &block, int pos)
{
    if (pos < 0 || pos >= block.length())
        return false;
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    if (!ud)
        return false;
    return shouldSkip(ud, pos);
}

QList<BTextBlockUserData::SkipSegment> BTextBlockUserData::skipSegments(const QTextBlock &block)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    return ud ? ud->d_func()->skipSegments : QList<SkipSegment>();
}

QString BTextBlockUserData::textWithoutSkipSegments(const BTextBlockUserData *ud, const QString &text, char replacer)
{
    if (!ud)
        return text;
    QString ntext = text;
    foreach (int i, bRangeR(ud->d_func()->skipSegments.size() - 1, 0)) {
        const SkipSegment &s = ud->d_func()->skipSegments.at(i);
        if (s.start >= ntext.length() || s.end >= ntext.length())
            continue;
        int len = s.end - s.start + 1;
        if ('\0' != replacer)
            ntext.replace(s.start, len, QString().fill(replacer, len));
        else
            ntext.remove(s.start, len);
    }
    return ntext;
}

QString BTextBlockUserData::textWithoutSkipSegments(const QTextBlock &block, char replacer)
{
    return textWithoutSkipSegments(static_cast<BTextBlockUserData *>(block.userData()), block.text(), replacer);
}

/*============================== Public methods ============================*/

BTextBlockExtraData *BTextBlockUserData::extraData() const
{
    return d_func()->data;
}

void BTextBlockUserData::setExtraData(BTextBlockExtraData *data)
{
    if (data != d_func()->data)
        delete d_func()->data;
    d_func()->data = data;
}

void BTextBlockUserData::setSkipSegments(const QList<SkipSegment> &list)
{
    d_func()->skipSegments = BTextBlockUserDataPrivate::processList(list);
}

QList<BTextBlockUserData::SkipSegment> BTextBlockUserData::skipSegments() const
{
    return d_func()->skipSegments;
}

/*============================================================================
================================ BSyntaxHighlighter ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BSyntaxHighlighter::BSyntaxHighlighter(BAbstractCodeEditorDocument *doc, QTextDocument *parent) :
    QSyntaxHighlighter(parent), EditorDocument(doc)
{
    //
}

/*============================== Public methods ============================*/

QTextBlock BSyntaxHighlighter::currentBlock() const
{
    return QSyntaxHighlighter::currentBlock();
}

BTextBlockExtraData *BSyntaxHighlighter::currentBlockExtraData() const
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(QSyntaxHighlighter::currentBlockUserData());
    return ud ? ud->extraData() : 0;
}

int BSyntaxHighlighter::currentBlockState() const
{
    return QSyntaxHighlighter::currentBlockState();
}

BTextBlockUserData *BSyntaxHighlighter::currentBlockUserData() const
{
    return static_cast<BTextBlockUserData *>(QSyntaxHighlighter::currentBlockUserData());
}

BAbstractCodeEditorDocument *BSyntaxHighlighter::editorDocument() const
{
    return EditorDocument;
}

QTextCharFormat BSyntaxHighlighter::format(int position) const
{
    return QSyntaxHighlighter::format(position);
}

int BSyntaxHighlighter::previousBlockState() const
{
    return QSyntaxHighlighter::previousBlockState();
}

void BSyntaxHighlighter::setCurrentBlockExtraData(BTextBlockExtraData *data)
{
    BTextBlockUserData *ud = currentBlockUserData();
    if (!ud) {
        ud = new BTextBlockUserData;
        setCurrentBlockUserData(ud);
    }
    ud->setExtraData(data);
}

void BSyntaxHighlighter::setCurrentBlockState(int newState)
{
    QSyntaxHighlighter::setCurrentBlockState(newState);
}

void BSyntaxHighlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
    QSyntaxHighlighter::setFormat(start, count, format);
}

void BSyntaxHighlighter::setFormat(int start, int count, const QColor &color)
{
    QSyntaxHighlighter::setFormat(start, count, color);
}

void BSyntaxHighlighter::setFormat(int start, int count, const QFont &font)
{
    QSyntaxHighlighter::setFormat(start, count, font);
}

/*============================== Protected methods =========================*/

void BSyntaxHighlighter::highlightBlock(const QString &text)
{
    BAbstractFileType *ft = EditorDocument ? EditorDocument->fileType() : 0;
    if (ft) {
        ft->d_func()->highlighter = this;
        ft->highlightBlock(text);
        ft->d_func()->highlighter = 0;
    }
    BSpellChecker *sc = EditorDocument ? EditorDocument->spellChecker() : 0;
    if (!sc)
        return;
    int i = 0;
    QString w;
    while (i <= text.length()) {
        if (i < text.length() && (text.at(i).isLetterOrNumber() || text.at(i) == '_')) {
            w += text.at(i);
        } else if (!w.isEmpty()) {
            QString sleft;
            int j = i - w.length() - 1;
            while (j >= 0 && !text.at(j).isSpace())
                sleft.prepend(text.at(j--));
            QString sright;
            j = i;
            while (j < text.length() && !text.at(j).isSpace())
                sright.append(text.at(j++));
            if (!sc->spell(w, sleft, sright)) {
                foreach (int j, bRangeD(i - w.length(), i - 1)) {
                    QTextCharFormat fmt = format(j);
                    fmt.setUnderlineColor(QColor("red"));
                    fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                    setFormat(j, 1, fmt);
                }
            }
            w.clear();
        }
        ++i;
    }
}

/*============================================================================
================================ BAbstractCodeEditorDocumentPrivate ==========
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractCodeEditorDocumentPrivate::BAbstractCodeEditorDocumentPrivate(BAbstractCodeEditorDocument *q,
                                                                       BCodeEditor *editor) :
    BBaseObjectPrivate(q), Editor(editor)
{
    //
}

BAbstractCodeEditorDocumentPrivate::~BAbstractCodeEditorDocumentPrivate()
{
    //
}

/*============================== Static public methods =====================*/

void BAbstractCodeEditorDocumentPrivate::addBlockSkipSegment(QTextBlock block, const SkipSegment &s)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    QList<BAbstractFileType::SkipSegment> list;
    list << s;
    if (ud) {
        list << ud->skipSegments();
        ud->setSkipSegments(list);
    } else {
        ud = new BTextBlockUserData(list);
        block.setUserData(ud);
    }
}

void BAbstractCodeEditorDocumentPrivate::addBlockSkipSegment(QTextBlock block, int start, int end)
{
    BAbstractFileType::SkipSegment s;
    s.start = start;
    s.end = (end >= 0) ? end : (block.text().length() - 1);
    addBlockSkipSegment(block, s);
}

void BAbstractCodeEditorDocumentPrivate::addBlockSkipSegmentL(QTextBlock block, int start, int length)
{
    BAbstractFileType::SkipSegment s;
    s.start = start;
    s.end = (length >= 0) ? ((s.start >= 0) ? (s.start + length - 1) : -1) : (block.text().length() - 1);
    addBlockSkipSegment(block, s);
}

QTextCharFormat BAbstractCodeEditorDocumentPrivate::createBracketsErrorFormat()
{
    QTextCharFormat fmt;
    fmt.setBackground(QBrush(QColor("hotpink")));
    return fmt;
}

QTextCharFormat BAbstractCodeEditorDocumentPrivate::createBracketsFormat()
{
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor("red")));
    return fmt;
}

BAbstractCodeEditorDocumentPrivate::FindBracketPairResult
    BAbstractCodeEditorDocumentPrivate::createFindBracketPairResult()
{
    FindBracketPairResult r;
    r.start = -1;
    r.end = -1;
    r.startBr = 0;
    r.endBr = 0;
    return r;
}

QTextCharFormat BAbstractCodeEditorDocumentPrivate::createSearchResultFormat()
{
    QTextCharFormat fmt;
    fmt.setBackground(QBrush(QColor("yellow")));
    return fmt;
}

int BAbstractCodeEditorDocumentPrivate::removeExtraSelections(ExtraSelectionList &from, const SelectionRangeList &what)
{
    int count = 0;
    bool range = false;
    bool hasBegin = false;
    ExtraSelectionList::Iterator begin = 0;
    ExtraSelectionList::Iterator end = 0;
    for (ExtraSelectionList::Iterator i = from.begin(); i != from.end(); ++i) {
        const QTextCursor &tc = i->cursor;
        int sstart = qMin(tc.selectionStart(), tc.selectionEnd());
        int send = qMax(tc.selectionStart(), tc.selectionEnd());
        bool found = false;
        foreach (const SelectionRange &sr, what) {
            if (sr.start == sstart && sr.end == send) {
                range = true;
                if (!hasBegin) {
                    hasBegin = true;
                    begin = i;
                }
                end = i + 1;
                ++count;
                found = true;
                break;
            }
        }
        if (!found && range) {
            from.erase(begin, end);
            range = false;
            hasBegin = false;
        }
    }
    if (range) {
        from.erase(begin, end);
    }
    return count;
}

BAbstractCodeEditorDocumentPrivate::SelectionRangeList BAbstractCodeEditorDocumentPrivate::removeSelectionRanges(
        SelectionRangeList &from, int start, int end)
{
    SelectionRangeList list;
    foreach (int i, bRangeR(from.size() - 1, 0)) {
        const SelectionRange &sr = from.at(i);
        if (sr.start >= start && sr.end <= end)
            list << from.takeAt(i);
    }
    return list;
}

bool BAbstractCodeEditorDocumentPrivate::selectionRangeListsEqual(const SelectionRangeList &list1,
                                                                  const SelectionRangeList &list2)
{
    if (list1.size() != list2.size())
        return false;
    if (list1.isEmpty())
        return true;
    foreach (int i, bRangeD(0, list1.size() - 1)) {
        const SelectionRange &sr1 = list1.at(i);
        const SelectionRange &sr2 = list2.at(i);
        if (sr1.start != sr2.start || sr1.end != sr2.end)
            return false;
    }
    return true;
}

void BAbstractCodeEditorDocumentPrivate::setBlockSkipSegments(QTextBlock block, const QList<SkipSegment> &list)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    if (!ud) {
        ud = new BTextBlockUserData(list);
        block.setUserData(ud);
    }
    ud->setSkipSegments(list);
}

/*============================== Public methods ============================*/

bool BAbstractCodeEditorDocumentPrivate::createEdit()
{
    edit = q_func()->createEdit(&document);
    if (!edit) {
        document = 0;
        return false;
    }
    static_cast<QVBoxLayout *>(q_func()->layout())->addWidget(edit);
    highlighter = new BSyntaxHighlighter(q_func(), document);
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(edit);
    if (cedt) {
        cedt->innerEdit()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(cedt->innerEdit(), SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(customContextMenuRequested(QPoint)));
    } else {
        edit->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(edit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    }
    edit->installEventFilter(this);
    return true;
}

QMenu *BAbstractCodeEditorDocumentPrivate::createSpellCheckerMenu(const QPoint &pos)
{
    if (!spellChecker || !edit)
        return 0;
    QTextEdit *tedt = qobject_cast<QTextEdit *>(edit);
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(edit);
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(edit);
    QTextCursor tc;
    if (tedt)
        tc = tedt->cursorForPosition(pos);
    else if (ptedt)
        tc = ptedt->cursorForPosition(pos);
    else if (cedt)
        tc = cedt->innerEdit()->cursorForPosition(pos);
    if (tc.isNull())
        return 0;
    QString text = tc.block().text();
    int p = tc.positionInBlock();
    QString w;
    int i = p;
    while (i >= 0 && (text.at(i).isLetterOrNumber() || text.at(i) == '_'))
        w.prepend(text.at(i--));
    int wbl = i;
    wordToReplace.first = tc.block().position() + i + 1;
    i = p + 1;
    while (i < text.length() && (text.at(i).isLetterOrNumber() || text.at(i) == '_'))
        w.append(text.at(i++));
    int wbr = i;
    wordToReplace.second = wordToReplace.first + w.length();
    if (w.isEmpty())
        return 0;
    QString sleft;
    i = wbl;
    while (i >= 0 && !text.at(i).isSpace())
        sleft.prepend(text.at(i--));
    QString sright;
    i = wbr;
    while (i < text.length() && !text.at(i).isSpace())
        sright.append(text.at(i++));
    QMenu *mnu = new QMenu(tr("Spell check", "mnu title"));
    mnu->setEnabled(!readOnly);
    QStringList suggestions = spellChecker->suggest(w);
    while (suggestions.size() > 10)
        suggestions.removeLast();
    foreach (const QString &s, suggestions)
        mnu->addAction(s, this, SLOT(replaceWord()));
    if (!suggestions.isEmpty())
        mnu->addSeparator();
    bool implicitly = false;
    if (spellChecker->isIgnored(&implicitly, w, sleft, sright) && !implicitly)
        mnu->addAction(tr("Remove from ignore list", "act text"), this,
                       SLOT(dontIgnoreWord()))->setProperty("beqt/word", w);
    else if (!spellChecker->spell(w))
        mnu->addAction(tr("Ignore this word", "act text"), this, SLOT(ignoreWord()))->setProperty("beqt/word", w);
    if (mnu->isEmpty()) {
        delete mnu;
        mnu = 0;
    }
    return mnu;
}

bool BAbstractCodeEditorDocumentPrivate::eventFilter(QObject *, QEvent *e)
{
    if (e->type() != QEvent::ToolTip)
        return false;
    QHelpEvent* he = static_cast<QHelpEvent*>(e);
    toolTipTimer.stop();
    QTextCursor tc = q_func()->textCursorForPosition(he->pos());
    if (tc.isNull())
        return true;
    toolTipCursor = tc;
    toolTipGlobalPos = he->globalPos();
    toolTipTimer.start();
    return true;
}

BAbstractCodeEditorDocumentPrivate::FindBracketPairResult
    BAbstractCodeEditorDocumentPrivate::findLeftBracketPair() const
{
    QTextCursor tc = q_func()->textCursor();
    FindBracketPairResult res = createFindBracketPairResult();
    if (tc.isNull())
        return res;
    QTextBlock tb = tc.block();
    int posInBlock = tc.positionInBlock();
    const BracketPair *bracket = 0;
    if (!testBracket(BTextBlockUserData::textWithoutSkipSegments(tb, ' '), posInBlock, false, bracket))
        return res;
    res.end = tb.position() + posInBlock;
    posInBlock -= bracket->closing.length();
    int depth = 1;
    const BracketPair *br = 0;
    while (tb.isValid()) {
        QString text = BTextBlockUserData::textWithoutSkipSegments(tb, ' ');
        while (posInBlock >= 0) {
            if (testBracket(text, posInBlock, true, br)) {
                --depth;
                if (!depth) {
                    res.start = tb.position() + posInBlock;
                    res.startBr = br;
                    res.endBr = bracket;
                    return res;
                }
                if (testBracket(text, posInBlock, false, br)) {
                    ++depth;
                    posInBlock -= br->closing.length();
                } else {
                    --posInBlock;
                }
            } else if (testBracket(text, posInBlock, false, br)) {
                ++depth;
                posInBlock -= br->closing.length();
            } else {
                --posInBlock;
            }
        }
        tb = tb.previous();
        posInBlock = tb.length();
    }
    return res;
}

BAbstractCodeEditorDocumentPrivate::FindBracketPairResult
    BAbstractCodeEditorDocumentPrivate::findRightBracketPair() const
{
    QTextCursor tc = q_func()->textCursor();
    FindBracketPairResult res = createFindBracketPairResult();
    if (tc.isNull())
        return res;
    QTextBlock tb = tc.block();
    int posInBlock = tc.positionInBlock();
    const BracketPair *bracket = 0;
    if (!testBracket(BTextBlockUserData::textWithoutSkipSegments(tb, ' '), posInBlock, true, bracket))
        return res;
    res.start = tb.position() + posInBlock;
    posInBlock += bracket->opening.length();
    int depth = 1;
    const BracketPair *br = 0;
    while (tb.isValid()) {
        QString text = BTextTools::removeTrailingSpaces(BTextBlockUserData::textWithoutSkipSegments(tb, ' '));
        while (posInBlock <= text.length()) {
            if (testBracket(text, posInBlock, false, br)) {
                --depth;
                if (!depth) {
                    res.end = tb.position() + posInBlock;
                    res.startBr = bracket;
                    res.endBr = br;
                    return res;
                }
                if (testBracket(text, posInBlock, true, br)) {
                    ++depth;
                    posInBlock += br->opening.length();
                } else {
                    ++posInBlock;
                }
            } else if (testBracket(text, posInBlock, true, br)) {
                ++depth;
                posInBlock += br->opening.length();
            } else {
                ++posInBlock;
            }
        }
        tb = tb.next();
        posInBlock = 0;
    }
    return res;
}

void BAbstractCodeEditorDocumentPrivate::highlightBrackets()
{
    SelectionRangeList newSelections;
    SelectionRangeList newSelectionsError;
    if (bracketsHighlighting) {
        FindBracketPairResultList list;
        list << findLeftBracketPair();
        list << findRightBracketPair();
        foreach (const FindBracketPairResult &res, list) {
            if (res.start >= 0 && res.end >= 0) {
                if (BAbstractFileType::areEqual(*res.startBr, *res.endBr)) {
                    SelectionRange srs;
                    srs.start = res.start;
                    srs.end = res.start + res.startBr->opening.length();
                    newSelections << srs;
                    SelectionRange sre;
                    sre.start = res.end - res.endBr->closing.length();
                    sre.end = res.end;
                    newSelections << sre;
                } else {
                    SelectionRange sr;
                    sr.start = res.start;
                    sr.end = res.end;
                    newSelectionsError << sr;
                }
            }
        }
    }
    if (selectionRangeListsEqual(newSelections, highlightedBrackets)
            && selectionRangeListsEqual(newSelectionsError, highlightedBracketsError)) {
        return;
    }
    ExtraSelectionList selections = q_func()->extraSelections();
    highlightedBrackets.clear();
    highlightedBracketsError.clear();
    if (bracketsHighlighting) {
        highlightedBrackets = newSelections;
        highlightedBracketsError = newSelectionsError;
    }
    q_func()->setExtraSelections(selections);
}

void BAbstractCodeEditorDocumentPrivate::init()
{
    QVBoxLayout *vlt = new QVBoxLayout(q_func());
      vlt->setContentsMargins(0, 0, 0, 0);
    edit = 0;
    document = 0;
    highlighter = 0;
    fileType = BAbstractFileType::defaultFileType();
    recognizedBrackets = fileType->brackets();
    bracketsHighlighting = true;
    spellChecker = 0;
    codec = QTextCodec::codecForName("UTF-8");
    asyncMin = 100 * 1000;
    readOnly = false;
    isModified = false;
    hasSelection = false;
    cutAvailable = false;
    copyAvailable = false;
    pasteAvailable = false;
    undoAvailable = false;
    redoAvailable = false;
    buisy = false;
    cursorPosition = 0;
    autocompletionTimer.setSingleShot(true);
    autocompletionTimer.setInterval(500);
    connect(&autocompletionTimer, SIGNAL(timeout()), this, SLOT(showAutocompletionMenu()));
    toolTipTimer.setSingleShot(true);
    toolTipTimer.setInterval(500);
    connect(&toolTipTimer, SIGNAL(timeout()), this, SLOT(showToolTip()));
}

void BAbstractCodeEditorDocumentPrivate::rehighlight()
{
    if (!highlighter)
        return;
    highlighter->rehighlight();
    highlightBrackets();
}

void BAbstractCodeEditorDocumentPrivate::rehighlightSearchResults()
{
    if ((!lastSearchRegexp.isValid() || lastSearchRegexp.isEmpty()) && lastSearchText.isEmpty()) {
        if (!highlightedSearchResults.isEmpty()) {
            highlightedSearchResults.clear();
            q_func()->setExtraSelections(q_func()->extraSelections());
        }
        return;
    }
    B_Q(BAbstractCodeEditorDocument);
    ExtraSelectionList selections = q->extraSelections();
    highlightedSearchResults.clear();
    if (lastSearchRegexp.isValid() && !lastSearchRegexp.isEmpty()) {
        QStringList lines = q->text(true).split('\n');
        int len = 0;
        foreach (int i, bRangeD(0, lines.size() - 1)) {
            const QString &t = lines.at(i);
            int ind = lastSearchRegexp.indexIn(t);
            while (ind >= 0) {
                SelectionRange sr;
                sr.start = ind + len;
                sr.end = sr.start + lastSearchRegexp.matchedLength();
                highlightedSearchResults << sr;
                ind = lastSearchRegexp.indexIn(t, ind + lastSearchRegexp.matchedLength());
            }
            len += t.length() + 1;
        }
    } else {
        Qt::CaseSensitivity cs = (lastSearchFlags & QTextDocument::FindCaseSensitively) ? Qt::CaseSensitive :
                                                                                          Qt::CaseInsensitive;
        bool wholeWords = lastSearchFlags & QTextDocument::FindWholeWords;
        QString t = q->text(true);
        int ind = BTextTools::indexOf(t, lastSearchText, 0, cs, wholeWords);
        while (ind >= 0) {
            SelectionRange sr;
            sr.start = ind;
            sr.end = ind + lastSearchText.length();
            highlightedSearchResults << sr;
            ind = BTextTools::indexOf(t, lastSearchText, ind + lastSearchText.length(), cs, wholeWords);
        }
    }
    q->setExtraSelections(selections);
}

void BAbstractCodeEditorDocumentPrivate::setBuisy(bool b)
{
    if (b == buisy)
        return;
    buisy = b;
    QMetaObject::invokeMethod(q_func(), "buisyChanged", Q_ARG(bool, b));
    if (!b)
        QMetaObject::invokeMethod(q_func(), "allProcessingFinished");
}

void BAbstractCodeEditorDocumentPrivate::setCodec(QTextCodec *c)
{
    if (!c)
        return;
    bool b = (c != codec);
    codec = c;
    if (b)
        QMetaObject::invokeMethod(q_func(), "codecChanged", Q_ARG(QString, BeQt::codecName(c)));
}

void BAbstractCodeEditorDocumentPrivate::setFileName(QString fn)
{
    fn = QDir::fromNativeSeparators(fn);
    bool b = (fn != fileName);
    fileName = fn;
    if (b)
        QMetaObject::invokeMethod(q_func(), "fileNameChanged", Q_ARG(QString, fn));
}

bool BAbstractCodeEditorDocumentPrivate::testBracket(const QString &text, int posInBlock, bool opening,
                                                     const BracketPair *&bracket) const
{
    bracket = 0;
    if (recognizedBrackets.isEmpty())
        return false;
    foreach (const BracketPair &br, recognizedBrackets) {
        const QString &brText = opening ? br.opening : br.closing;
        if (brText.isEmpty())
            continue;
        int len = brText.length();
        int pos = opening ? posInBlock : posInBlock - len;
        if (text.mid(pos, len) != brText)
            continue;
        if (br.escape.isEmpty()) {
            bracket = &br;
            return true;
        }
        len = br.escape.length();
        if (text.mid(pos - len, len) != br.escape) {
            bracket = &br;
            return true;
        }
    }
    return false;
}

/*============================== Public slots ==============================*/

void BAbstractCodeEditorDocumentPrivate::customContextMenuRequested(const QPoint &pos)
{
    if (!edit)
        return;
    QMenu *mnu = q_func()->createContextMenu();
    if (!mnu)
        return;
    QMenu *scmnu = createSpellCheckerMenu(pos);
    if (scmnu) {
        mnu->addSeparator();
        mnu->addMenu(scmnu);
    }
    mnu->exec(edit->mapToGlobal(pos));
    delete mnu;
}

void BAbstractCodeEditorDocumentPrivate::dontIgnoreWord()
{
    if (!spellChecker)
        return;
    QAction *act = qobject_cast<QAction *>(sender());
    QString text = act ? act->property("beqt/word").toString() : QString();
    if (text.isEmpty())
        return;
    spellChecker->ignoreWord(text, false);
    highlighter->rehighlight();
}

void BAbstractCodeEditorDocumentPrivate::ignoreWord()
{
    if (!spellChecker)
        return;
    QAction *act = qobject_cast<QAction *>(sender());
    QString text = act ? act->property("beqt/word").toString() : QString();
    if (text.isEmpty())
        return;
    spellChecker->ignoreWord(text);
    highlighter->rehighlight();
}

void BAbstractCodeEditorDocumentPrivate::loadingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                         bool success, const QString &text)
{
    if (operation.document != q_func())
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>(sender());
    disconnect(driver, SIGNAL(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)),
               this, SLOT(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)));
    if (success) {
        if (!operation.fileName.isEmpty())
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        q_func()->setText(text);
        q_func()->setModification(false);
        q_func()->clearUndoRedoStacks(QTextDocument::UndoAndRedoStacks);
    }
    setBuisy(false);
    QMetaObject::invokeMethod(q_func(), "loadingFinished", Q_ARG(bool, success));
}

void BAbstractCodeEditorDocumentPrivate::preprocessingFinished()
{
    TextProcessingResultFutureWatcher *watcher = dynamic_cast<TextProcessingResultFutureWatcher *>(sender());
    if (!watcher)
        return;
    TextProcessingResult res = watcher->result();
    delete watcher;
    edit->setEnabled(true);
    q_func()->blockHighlighter(true);
    q_func()->setTextImplementation(res.text);
    q_func()->blockHighlighter(false);
    q_func()->setFocusImplementation();
    q_func()->moveCursor(QPoint(0, 0));
    setBuisy(false);
    q_func()->afterPreprocessing(res.userData);
}

void BAbstractCodeEditorDocumentPrivate::replaceWord()
{
    if (readOnly)
        return;
    QAction *act = qobject_cast<QAction *>(sender());
    QString text = act ? act->text() : QString();
    if (text.isEmpty())
        return;
    q_func()->selectTextImplementation(wordToReplace.first, wordToReplace.second);
    if (!hasSelection)
        return;
    q_func()->insertTextImplementation(act->text());
}

void BAbstractCodeEditorDocumentPrivate::savingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                        bool success)
{
    if (operation.document != q_func())
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>(sender());
    disconnect(driver, SIGNAL(savingFinished(BAbstractDocumentDriver::Operation, bool)),
               this, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool)));
    if (success) {
        if (!operation.fileName.isEmpty())
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        q_func()->setModification(false);
    }
    setBuisy(false);
    QMetaObject::invokeMethod(q_func(), "savingFinished", Q_ARG(bool, success));
}

void BAbstractCodeEditorDocumentPrivate::showAutocompletionMenu()
{
    if (!fileType)
        return;
    fileType->showAutocompletionMenu(q_func(), autocompletionCursor, autocompletionGlobalPos);
}

void BAbstractCodeEditorDocumentPrivate::showToolTip()
{
    if (!fileType)
        return;
    fileType->showToolTip(q_func(), toolTipCursor, toolTipGlobalPos);
}

/*============================================================================
================================ BAbstractCodeEditorDocument =================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractCodeEditorDocument::BAbstractCodeEditorDocument(BCodeEditor *editor, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BAbstractCodeEditorDocumentPrivate(this, editor))
{
    d_func()->init();
}

BAbstractCodeEditorDocument::~BAbstractCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractCodeEditorDocument::BAbstractCodeEditorDocument(BAbstractCodeEditorDocumentPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

int BAbstractCodeEditorDocument::asyncProcessingMinimumLength() const
{
    return d_func()->asyncMin;
}

void BAbstractCodeEditorDocument::blockHighlighter(bool block)
{
    if (!d_func()->highlighter)
        return;
    if (block)
        d_func()->highlighter->setDocument(0);
    else
        d_func()->highlighter->setDocument(innerDocument());
}

QTextCodec *BAbstractCodeEditorDocument::codec() const
{
    return d_func()->codec;
}

QString BAbstractCodeEditorDocument::codecName() const
{
    return BeQt::codecName(d_func()->codec);
}

QMenu *BAbstractCodeEditorDocument::createContextMenu()
{
    QTextEdit *tedt = qobject_cast<QTextEdit *>(innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(innerEdit());
    if (tedt)
        return tedt->createStandardContextMenu();
    else if (ptedt)
        return ptedt->createStandardContextMenu();
    else if (cedt)
        return cedt->createContextMenu();
    else
        return 0;
}

BAbstractCodeEditorDocument::ExtraSelection BAbstractCodeEditorDocument::createExtraSelection(
        const QTextCharFormat &format) const
{
    ExtraSelection r;
    r.cursor = textCursor();
    if (r.cursor.isNull())
        return ExtraSelection();
    r.format = format;
    return r;
}

int BAbstractCodeEditorDocument::cursorPosition() const
{
    return d_func()->cursorPosition;
}

QPoint BAbstractCodeEditorDocument::cursorPositionRowColumn() const
{
    return d_func()->cursorPositionRowColumn;
}

QRect BAbstractCodeEditorDocument::cursorRect() const
{
    QTextEdit *tedt = qobject_cast<QTextEdit *>(innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(innerEdit());
    if (tedt)
        return tedt->cursorRect();
    else if (ptedt)
        return ptedt->cursorRect();
    else if (cedt)
        return cedt->innerEdit()->cursorRect();
    else
        return QRect();
}

BCodeEditor *BAbstractCodeEditorDocument::editor() const
{
    return d_func()->Editor;
}

BAbstractCodeEditorDocument::ExtraSelectionList BAbstractCodeEditorDocument::extraSelections() const
{
    ExtraSelectionList &list = const_cast<BAbstractCodeEditorDocument *>(this)->d_func()->userSelections;
    foreach (int i, bRangeR(list.size() - 1, 0)) {
        if (!list.at(i).cursor.hasSelection())
            list.removeAt(i);
    }
    return list;
}

QString BAbstractCodeEditorDocument::fileName() const
{
    return d_func()->fileName;
}

BAbstractFileType *BAbstractCodeEditorDocument::fileType() const
{
    return d_func()->fileType;
}

QString BAbstractCodeEditorDocument::fileTypeId() const
{
    return d_func()->fileType ? d_func()->fileType->id() : QString();
}

bool BAbstractCodeEditorDocument::findNext(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    B_D(BAbstractCodeEditorDocument);
    bool b = findNextImplementation(txt, flags, cyclic);
    d->lastSearchFlags = flags;
    d->lastSearchRegexp = QRegExp();
    d->lastSearchText = txt;
    d->rehighlightSearchResults();
    return b;
}

bool BAbstractCodeEditorDocument::findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags, bool cyclic)
{
    B_D(BAbstractCodeEditorDocument);
    bool b = findNextRegexpImplementation(rx, flags, cyclic);
    d->lastSearchFlags = flags;
    d->lastSearchRegexp = rx;
    d->lastSearchText.clear();
    d->rehighlightSearchResults();
    return b;
}

bool BAbstractCodeEditorDocument::hasSelection() const
{
    return d_func()->hasSelection;
}

void BAbstractCodeEditorDocument::init()
{
    if (d_func()->edit)
        return;
    d_func()->createEdit();
}

QTextDocument *BAbstractCodeEditorDocument::innerDocument() const
{
    return d_func()->document;
}

QWidget *BAbstractCodeEditorDocument::innerEdit(QTextDocument **doc) const
{
    if (doc)
        *doc = d_func()->highlighter ? d_func()->highlighter->document() : 0;
    return d_func()->edit;
}

bool BAbstractCodeEditorDocument::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

bool BAbstractCodeEditorDocument::isBuisy() const
{
    return d_func()->buisy;
}

bool BAbstractCodeEditorDocument::isCopyAvailable() const
{
    return d_func()->copyAvailable;
}

bool BAbstractCodeEditorDocument::isCutAvailable() const
{
    return d_func()->cutAvailable;
}

bool BAbstractCodeEditorDocument::isModified() const
{
    return d_func()->isModified;
}

bool BAbstractCodeEditorDocument::isPasteAvailable() const
{
    return d_func()->pasteAvailable;
}

bool BAbstractCodeEditorDocument::isReadOnly() const
{
    return d_func()->readOnly;
}

bool BAbstractCodeEditorDocument::isRedoAvailable() const
{
    return d_func()->redoAvailable;
}

bool BAbstractCodeEditorDocument::isUndoAvailable() const
{
    return d_func()->undoAvailable;
}

bool BAbstractCodeEditorDocument::load(BAbstractDocumentDriver *driver, const QString &fileName)
{
    return load(driver, 0, fileName);
}

bool BAbstractCodeEditorDocument::load(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName)
{
    if (!driver || isBuisy())
        return false;
    B_D(BAbstractCodeEditorDocument);
    d->setBuisy(true);
    connect(driver, SIGNAL(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)),
            d,SLOT(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)));
    if (!driver->load(this, codec, fileName)) {
        disconnect(driver, SIGNAL(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)),
                   d, SLOT(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)));
        d->setBuisy(false);
        return false;
    }
    return true;
}

BAbstractCodeEditorDocument::BracketPairList BAbstractCodeEditorDocument::recognizedBrackets() const
{
    return d_func()->recognizedBrackets;
}

bool BAbstractCodeEditorDocument::save(BAbstractDocumentDriver *driver, const QString &fileName)
{
    return save(driver, 0, fileName);
}

bool BAbstractCodeEditorDocument::save(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName)
{
    return save(driver, codec, BeQt::DefaultLineFeed, fileName);
}

bool BAbstractCodeEditorDocument::save(BAbstractDocumentDriver *driver, QTextCodec *codec, BeQt::LineFeed lineFeed,
                                       const QString &fileName)
{
    if (!driver || isBuisy())
        return false;
    B_D(BAbstractCodeEditorDocument);
    d->setBuisy(true);
    connect(driver, SIGNAL(savingFinished(BAbstractDocumentDriver::Operation, bool)),
            d, SLOT(savingFinished(BAbstractDocumentDriver::Operation, bool)));
    if (!driver->save(this, codec, lineFeed, fileName)) {
        disconnect(driver, SIGNAL(savingFinished(BAbstractDocumentDriver::Operation, bool)),
                   d, SLOT(savingFinished(BAbstractDocumentDriver::Operation, bool)));
        d->setBuisy(false);
        return false;
    }
    return true;
}

void BAbstractCodeEditorDocument::setAsyncProcessingMinimumLength(int length)
{
    d_func()->asyncMin = (length > 0) ? length : 0;
}

void BAbstractCodeEditorDocument::setBracketHighlightingEnabled(bool enabled)
{
    if (enabled == d_func()->bracketsHighlighting)
        return;
    d_func()->bracketsHighlighting = enabled;
    d_func()->highlightBrackets();
}

void BAbstractCodeEditorDocument::setCodec(QTextCodec *codec)
{
    if (d_func()->buisy)
        return;
    d_func()->setCodec(codec);
}

void BAbstractCodeEditorDocument::setCodec(const QString &codecName)
{
    setCodec(BeQt::codec(codecName));
}

void BAbstractCodeEditorDocument::setExtraSelections(const ExtraSelectionList &list)
{
    B_D(BAbstractCodeEditorDocument);
    d->userSelections = list;
    foreach (int i, bRangeR(d->userSelections.size() - 1, 0)) {
        if (!d->userSelections.at(i).cursor.hasSelection())
            d->userSelections.removeAt(i);
    }
    ExtraSelectionList nlist = d->userSelections;
    ExtraSelection es = createExtraSelection(BAbstractCodeEditorDocumentPrivate::createSearchResultFormat());
    foreach (const BAbstractCodeEditorDocumentPrivate::SelectionRange &sr, d->highlightedSearchResults) {
        es.cursor.setPosition(sr.start);
        es.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        nlist << es;
    }
    es = createExtraSelection(BAbstractCodeEditorDocumentPrivate::createBracketsFormat());
    foreach (const BAbstractCodeEditorDocumentPrivate::SelectionRange &sr, d_func()->highlightedBrackets) {
        es.cursor.setPosition(sr.start);
        es.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        nlist << es;
    }
    es = createExtraSelection(BAbstractCodeEditorDocumentPrivate::createBracketsErrorFormat());
    foreach (const BAbstractCodeEditorDocumentPrivate::SelectionRange &sr, d_func()->highlightedBracketsError) {
        es.cursor.setPosition(sr.start);
        es.cursor.setPosition(sr.end, QTextCursor::KeepAnchor);
        nlist << es;
    }
    setExtraSelectionsImplementation(nlist);
}

void BAbstractCodeEditorDocument::setFileName(const QString &fn)
{
    if (d_func()->buisy)
        return;
    d_func()->setFileName(fn);
}

void BAbstractCodeEditorDocument::setFileType(BAbstractFileType *ft)
{
    B_D(BAbstractCodeEditorDocument);
    if (ft == d->fileType)
        return;
    if (ft && d->fileType && ft->id() == d->fileType->id())
        return;
    if (!ft && d->fileType && d->fileType->id() == "Text")
        return;
    d->fileType = ft ? ft : BAbstractFileType::defaultFileType();
    setRecognizedBrackets(ft->brackets());
    Q_EMIT fileTypeChanged(ft);
    rehighlight();
}

void BAbstractCodeEditorDocument::setRecognizedBrackets(const BracketPairList &list)
{
    if (BAbstractFileType::areEqual(d_func()->recognizedBrackets, list))
        return;
    d_func()->recognizedBrackets = list;
    d_func()->highlightBrackets();
}

void BAbstractCodeEditorDocument::setSpellChecker(BSpellChecker *sc)
{
    if (sc == d_func()->spellChecker)
        return;
    if (d_func()->spellChecker)
        disconnect(d_func()->spellChecker, SIGNAL(changed()), this, SLOT(rehighlight()));
    if (sc)
        connect(sc, SIGNAL(changed()), this, SLOT(rehighlight()));
    d_func()->spellChecker = sc;
    d_func()->highlighter->rehighlight();
}

bool BAbstractCodeEditorDocument::shouldProcessAsynchronously(const QString &txt) const
{
    return d_func()->asyncMin && txt.length() >= d_func()->asyncMin;
}

BSpellChecker *BAbstractCodeEditorDocument::spellChecker() const
{
    return d_func()->spellChecker;
}

QTextCursor BAbstractCodeEditorDocument::textCursor() const
{
    QTextEdit *tedt = qobject_cast<QTextEdit *>(innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(innerEdit());
    if (tedt)
        return tedt->textCursor();
    else if (ptedt)
        return ptedt->textCursor();
    else if (cedt)
        return cedt->innerEdit()->textCursor();
    else
        return QTextCursor();
}

QTextCursor BAbstractCodeEditorDocument::textCursorForPosition(const QPoint &pos) const
{
    QTextEdit *tedt = qobject_cast<QTextEdit *>(innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(innerEdit());
    if (tedt)
        return tedt->cursorForPosition(pos);
    else if (ptedt)
        return ptedt->cursorForPosition(pos);
    else if (cedt)
        return cedt->innerEdit()->cursorForPosition(pos);
    else
        return QTextCursor();
}

/*============================== Public slots ==============================*/

void BAbstractCodeEditorDocument::activateWindow()
{
    if (d_func()->edit)
        activateWindowImplementation();
}

void BAbstractCodeEditorDocument::clear()
{
    clearImplementation();
}

void BAbstractCodeEditorDocument::copy()
{
    copyImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::cut()
{
    cutImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::deleteSelection()
{
    deleteSelectionImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::deselectText()
{
    deselectTextImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::insertText(const QString &txt)
{
    insertTextImplementation(txt);
}

void BAbstractCodeEditorDocument::moveCursor(int pos)
{
    moveCursorImplementation(pos);
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::moveCursor(const QPoint &pos)
{
    moveCursorImplementation(pos);
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::paste()
{
    pasteImplementation();
}

void BAbstractCodeEditorDocument::redo()
{
    redoImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::rehighlight()
{
    d_func()->rehighlight();
}

void BAbstractCodeEditorDocument::rehighlightBlock(const QTextBlock &block)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->rehighlightBlock(block);
}

void BAbstractCodeEditorDocument::selectAll()
{
    selectAllImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::selectLines(int firstLine, int lastLine)
{
    selectLinesImplementation(firstLine, lastLine);
}

void BAbstractCodeEditorDocument::selectText(const QPoint &start, const QPoint &end)
{
    selectTextImplementation(start, end);
}

void BAbstractCodeEditorDocument::selectText(int start, int end)
{
    selectTextImplementation(start, end);
}

void BAbstractCodeEditorDocument::setFocus()
{
    if (d_func()->edit)
        setFocusImplementation();
}

void BAbstractCodeEditorDocument::setText(const QString &txt)
{
    if (txt.isEmpty())
       return setTextImplementation(txt);
    TextProcessingFunction f = textPreprocessingFunction();
    if (f && shouldProcessAsynchronously(txt)) {
        setBuisy(true);
        innerEdit()->setEnabled(false);
        setTextImplementation(tr("Processing content, please wait..."));
        BAbstractCodeEditorDocumentPrivate::TextProcessingResultFuture future =
                QtConcurrent::run(f, txt, textPreprocessingUserData());
        BAbstractCodeEditorDocumentPrivate::TextProcessingResultFutureWatcher *watcher =
                new BAbstractCodeEditorDocumentPrivate::TextProcessingResultFutureWatcher(this);
        watcher->setFuture(future);
        connect(watcher, SIGNAL(finished()), d_func(), SLOT(preprocessingFinished()));
    } else if (f) {
        TextProcessingResult res = f(txt, textPreprocessingUserData());
        blockHighlighter(true);
        setTextImplementation(res.text);
        blockHighlighter(false);
        moveCursorImplementation(QPoint(0, 0));
        setFocusImplementation();
        afterPreprocessing(res.userData);
    } else {
        blockHighlighter(true);
        setTextImplementation(txt);
        blockHighlighter(false);
    }
}

void BAbstractCodeEditorDocument::undo()
{
    undoImplementation();
    setFocusImplementation();
}

/*============================== Protected methods =========================*/

void BAbstractCodeEditorDocument::afterPreprocessing(const QVariant &)
{
    //
}

void BAbstractCodeEditorDocument::clearUndoRedoStacks(QTextDocument::Stacks historyToClear)
{
    QTextDocument *doc = innerDocument();
    if (!doc)
        return;
    doc->clearUndoRedoStacks(historyToClear);
}

BAbstractCodeEditorDocument::TextProcessingFunction BAbstractCodeEditorDocument::textPreprocessingFunction() const
{
    return 0;
}

QVariant BAbstractCodeEditorDocument::textPreprocessingUserData()
{
    return QVariant();
}

/*============================== Protected slots ===========================*/

void BAbstractCodeEditorDocument::emitSelectionChanged()
{
    Q_EMIT selectionChanged();
}

void BAbstractCodeEditorDocument::emitTextChanged()
{
    d_func()->rehighlightSearchResults();
    Q_EMIT textChanged();
}

void BAbstractCodeEditorDocument::setBuisy(bool buisy)
{
    d_func()->setBuisy(buisy);
}

void BAbstractCodeEditorDocument::setCopyAvailable(bool available)
{
    if (available == d_func()->copyAvailable)
        return;
    d_func()->copyAvailable = available;
    Q_EMIT copyAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setCursorPosition(int pos)
{
    B_D(BAbstractCodeEditorDocument);
    if (pos == d->cursorPosition)
        return;
    d->autocompletionTimer.stop();
    d->cursorPosition = pos;
    QPoint rc = cursorPositionRowColumnImplementation();
    bool b = rc != d->cursorPositionRowColumn;
    d->cursorPositionRowColumn = rc;
    d->highlightBrackets();
    Q_EMIT cursorPositionChanged(pos);
    if (b)
        Q_EMIT cursorPositionChanged(rc);
    QTextCursor tc = textCursor();
    if (tc.isNull())
        return;
    QRect r = cursorRect();
    if (!r.isValid() || r.isNull())
        return;
    QWidget *edt = innerEdit();
    if (!edt)
        return;
    d->autocompletionCursor = tc;
    d->autocompletionGlobalPos = edt->mapToGlobal(r.bottomRight());
    if (d->fileType && d->fileType->autocompletionMenuVisible(this))
        return d->showAutocompletionMenu();
    d->autocompletionTimer.start();
}

void BAbstractCodeEditorDocument::setCursorPosition(const QPoint &pos)
{
    setCursorPosition(cursorPositionForRowColumn(pos));
}

void BAbstractCodeEditorDocument::setCutAvailable(bool available)
{
    if (available == d_func()->cutAvailable)
        return;
    d_func()->cutAvailable = available;
    Q_EMIT cutAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setHasSelection(bool hasSelection)
{
    if (hasSelection == d_func()->hasSelection)
        return;
    d_func()->hasSelection = hasSelection;
    Q_EMIT hasSelectionChanged(hasSelection);
}

void BAbstractCodeEditorDocument::setModificationInternal(bool modified)
{
    if (modified == d_func()->isModified)
        return;
    d_func()->isModified = modified;
    Q_EMIT modificationChanged(modified);
}

void BAbstractCodeEditorDocument::setPasteAvailable(bool available)
{
    if (available == d_func()->pasteAvailable)
        return;
    d_func()->pasteAvailable = available;
    Q_EMIT pasteAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setReadOnlyInternal(bool ro)
{
    if (ro == d_func()->readOnly)
        return;
    d_func()->readOnly = ro;
    Q_EMIT readOnlyChanged(ro);
}

void BAbstractCodeEditorDocument::setRedoAvailable(bool available)
{
    if (available == d_func()->redoAvailable)
        return;
    d_func()->redoAvailable = available;
    Q_EMIT redoAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setUndoAvailable(bool available)
{
    if (available == d_func()->undoAvailable)
        return;
    d_func()->undoAvailable = available;
    Q_EMIT undoAvailableChanged(available);
}
