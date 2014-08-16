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

class BTextBlockExtraData;

#include "babstractfiletype.h"
#include "babstractfiletype_p.h"

#include "babstractcodeeditordocument_p.h"

#include <QAction>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QFont>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QToolTip>
#include <QVariant>

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

/*============================== Static protected variables ================*/

BDefaultFileType *BDefaultFileType::_m_self = 0;

/*============================== Public constructors =======================*/

BDefaultFileType::BDefaultFileType()
{
    //
}

BDefaultFileType::~BDefaultFileType()
{
    //
}

/*============================== Static public methods =====================*/

BDefaultFileType *BDefaultFileType::instance()
{
    if (!_m_self)
        _m_self = new BDefaultFileType;
    return _m_self;
}

/*============================== Public methods ============================*/

BAbstractFileType::BracketPairList BDefaultFileType::brackets() const
{
    return BracketPairList();
}

QString BDefaultFileType::description() const
{
    return tr("All files", "description");
}

QString BDefaultFileType::id() const
{
    return "Text";
}

bool BDefaultFileType::matchesFileName(const QString &) const
{
    return true;
}

QString BDefaultFileType::name() const
{
    return tr("Text", "name");
}

QStringList BDefaultFileType::suffixes() const
{
    return QStringList() << "*";
}

/*============================================================================
================================ BAbstractFileTypePrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractFileTypePrivate::BAbstractFileTypePrivate(BAbstractFileType *q) :
    BBasePrivate(q)
{
    //
}

BAbstractFileTypePrivate::~BAbstractFileTypePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractFileTypePrivate::init()
{
    highlighter = 0;
}

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractFileType::BAbstractFileType() :
    BBase(*new BAbstractFileTypePrivate(this))
{
    //
}

BAbstractFileType::~BAbstractFileType()
{
    //
}

/*============================== Static public methods =====================*/

bool BAbstractFileType::areEqual(const BracketPair &bp1, const BracketPair &bp2)
{
    return bp1.opening == bp2.opening && bp1.closing == bp2.closing && bp1.escape == bp2.escape;
}

bool BAbstractFileType::areEqual(const BracketPairList &l1, const BracketPairList &l2)
{
    if (l1.size() != l2.size())
        return false;
    for (int i = 0; i < l1.size(); ++i) {
        if (!areEqual(l1.at(i), l2.at(i)))
            return false;
    }
    return true;
}

BAbstractFileType *BAbstractFileType::defaultFileType()
{
    return BDefaultFileType::instance();
}

QString BAbstractFileType::defaultFileTypeId()
{
    return defaultFileType()->id();
}

/*============================== Public methods ============================*/

QString BAbstractFileType::createFileDialogFilter() const
{
    QString desc = description();
    QStringList sl = suffixes();
    if (desc.isEmpty() || sl.isEmpty())
        return "";
    QString filter;
    filter += desc + " (";
    if (sl.contains("*"))
        sl.clear();
    sl.removeDuplicates();
    if (!sl.isEmpty()) {
        for (int i = 0; i < sl.size(); ++i)
            filter += "*." + sl.at(i) + (i < sl.size() - 1 ? " " : "");
    }
    else
        filter += "*";
    filter += ")";
    return filter;
}

int BAbstractFileType::indentation(const QTextBlock &previousBlock) const
{
    if (!previousBlock.isValid())
        return 0;
    QString text = previousBlock.text();
    int i = 0;
    while (i < text.length() && text.at(i) == ' ')
        ++i;
    return i;
}

bool BAbstractFileType::matchesFileName(const QString &fileName) const
{
    return suffixes().contains(QFileInfo(fileName).suffix(), Qt::CaseInsensitive);
}

/*============================== Static protected methods ==================*/

BAbstractFileType::AutocompletionItem BAbstractFileType::createAutocompletionItem(
        const QString &text, const QString &actionText, const QString &actionToolTip, const QIcon &actionIcon)
{
    if (text.isEmpty())
        return AutocompletionItem();
    AutocompletionItem item;
    item.text = text;
    item.actionText = actionText;
    item.actionToolTip = actionToolTip;
    item.actionIcon = actionIcon;
    return item;
}

BAbstractFileType::BracketPair BAbstractFileType::createBracketPair(const QString &op, const QString &cl,
                                                                    const QString &esc)
{
    BracketPair bp;
    bp.opening = op;
    bp.closing = cl;
    bp.escape = esc;
    return bp;
}

/*============================== Protected methods =========================*/

void BAbstractFileType::addCurrentBlockSkipSegment(const SkipSegment &s)
{
    BAbstractCodeEditorDocumentPrivate::addBlockSkipSegment(currentBlock(), s);
}

void BAbstractFileType::addCurrentBlockSkipSegment(int start, int end)
{
    BAbstractCodeEditorDocumentPrivate::addBlockSkipSegment(currentBlock(), start, end);
}

void BAbstractFileType::addCurrentBlockSkipSegmentL(int start, int length)
{
    BAbstractCodeEditorDocumentPrivate::addBlockSkipSegmentL(currentBlock(), start, length);
}

void BAbstractFileType::clearCurrentBlockSkipSegments()
{
    BAbstractCodeEditorDocumentPrivate::setBlockSkipSegments(currentBlock(), QList<SkipSegment>());
}

QList<BAbstractFileType::AutocompletionItem> BAbstractFileType::createAutocompletionItemList(
        BAbstractCodeEditorDocument *, QTextCursor)
{
    return QList<AutocompletionItem>();
}

QString BAbstractFileType::createToolTipText(BAbstractCodeEditorDocument *, QTextCursor)
{
    return QString();
}

QTextBlock BAbstractFileType::currentBlock() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlock() : QTextBlock();
}

BTextBlockExtraData *BAbstractFileType::currentBlockExtraData() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlockExtraData() : 0;
}

int BAbstractFileType::currentBlockState() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlockState() : -1;
}

BAbstractCodeEditorDocument *BAbstractFileType::currentDocument() const
{
    if (!d_func()->highlighter)
        return 0;
    return d_func()->highlighter->EditorDocument;
}

QTextCharFormat BAbstractFileType::format(int position) const
{
    return d_func()->highlighter ? d_func()->highlighter->format(position) : QTextCharFormat();
}

void BAbstractFileType::highlightBlock(const QString &)
{
    //
}

int BAbstractFileType::previousBlockState() const
{
    return d_func()->highlighter ? d_func()->highlighter->previousBlockState() : -1;
}

void BAbstractFileType::setCurrentBlockExtraData(BTextBlockExtraData *data)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setCurrentBlockExtraData(data);
}

void BAbstractFileType::setCurrentBlockSkipSegments(const QList<SkipSegment> &list)
{
    BAbstractCodeEditorDocumentPrivate::setBlockSkipSegments(currentBlock(), list);
}

void BAbstractFileType::setCurrentBlockState(int newState)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setCurrentBlockState(newState);
}

void BAbstractFileType::setFormat(int start, int count, const QTextCharFormat &format)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setFormat(start, count, format);
}

void BAbstractFileType::setFormat(int start, int count, const QColor &color)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setFormat(start, count, color);
}

void BAbstractFileType::setFormat(int start, int count, const QFont &font)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setFormat(start, count, font);
}

void BAbstractFileType::showAutocompletionMenu(BAbstractCodeEditorDocument *doc, QTextCursor cursor,
                                               const QPoint &globalPos)
{
    if (cursor.isNull())
        return;
    cursor.select(QTextCursor::WordUnderCursor);
    if (!cursor.hasSelection())
        return;
    QList<AutocompletionItem> list = createAutocompletionItemList(doc, cursor);
    if (list.isEmpty())
        return;
    QMenu *mnu = new QMenu;
    foreach (const AutocompletionItem &aci, list) {
        if (aci.text.isEmpty() || aci.actionText.isEmpty())
            continue;
        QAction *act = mnu->addAction(aci.actionIcon, aci.actionText);
        act->setToolTip(aci.actionToolTip);
        act->setData(aci.text);
    }
    if (mnu->isEmpty()) {
        delete mnu;
        return;
    }
    mnu->setActiveAction(mnu->actions().first());
    QAction *act = mnu->exec(globalPos);
    if (!act) {
        delete mnu;
        return;
    }
    QString text = act->data().toString();
    delete mnu;
    doc->selectText(cursor.selectionStart(), cursor.selectionEnd());
    doc->insertText(text);
}

void BAbstractFileType::showToolTip(BAbstractCodeEditorDocument *doc, QTextCursor cursor, const QPoint &globalPos)
{
    QString text = createToolTipText(doc, cursor);
    if (text.isEmpty())
        return QToolTip::hideText();
    QToolTip::showText(globalPos, text);
}
