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

#include <QAbstractItemView>
#include <QAction>
#include <QColor>
#include <QCompleter>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QFont>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QModelIndex>
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QScrollBar>
#include <QSize>
#include <QStandardItem>
#include <QStandardItemModel>
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
================================ BAutoCompletionHelper =======================
============================================================================*/

/*============================== Public constructors =======================*/

BAutoCompletionHelper::BAutoCompletionHelper(BAbstractCodeEditorDocument *parent, QStandardItemModel *model) :
    QObject(parent), Doc(parent), Model(model)
{
    //
}

/*============================== Public methods ============================*/

bool BAutoCompletionHelper::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return false;
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    if (ke->key() != Qt::Key_Enter && ke->key() != Qt::Key_Return)
        return false;
    QAbstractItemView *popup = qobject_cast<QAbstractItemView *>(object);
    if (!popup || !popup->isVisible())
        return false;
    popup->hide();
    completerActivated(popup->currentIndex());
    ke->ignore();
    return true;
}

/*============================== Public slots ==============================*/

void BAutoCompletionHelper::completerActivated(const QModelIndex &index)
{
    if (!Doc || !Model || !index.isValid() || textCursor.isNull() || !textCursor.hasSelection())
        return;
    QStandardItem *item = Model->item(index.row());
    if (!item)
        return;
    Doc->selectText(textCursor.selectionStart(), textCursor.selectionEnd());
    Doc->insertText(item->data(Qt::UserRole).toString());
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

bool BAbstractFileType::autocompletionMenuVisible(BAbstractCodeEditorDocument *doc) const
{
    QCompleter *completer = doc ? doc->findChild<QCompleter *>("beqt/completer") : 0;
    return completer && completer->popup()->isVisible();
}

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

void BAbstractFileType::showAutocompletionMenu(BAbstractCodeEditorDocument *doc, QTextCursor cursor, const QPoint &)
{
    if (cursor.isNull())
        return;
    QCompleter *completer = doc->findChild<QCompleter *>("beqt/completer");
    cursor.select(QTextCursor::WordUnderCursor);
    if (!cursor.hasSelection()) {
        if (completer)
            completer->popup()->hide();
        return;
    }
    QList<AutocompletionItem> list = createAutocompletionItemList(doc, cursor);
    if (list.isEmpty()) {
        if (completer)
            completer->popup()->hide();
        return;
    }
    QStandardItemModel *model = doc->findChild<QStandardItemModel *>("beqt/completion_model");
    if (!model) {
        model = new QStandardItemModel(doc);
        model->setObjectName("beqt/completion_model");
        model->setColumnCount(1);
    }
    model->clear();
    foreach (const AutocompletionItem &item, list) {
        QStandardItem *si = new QStandardItem(item.actionIcon, item.actionText);
        si->setData(item.actionToolTip, Qt::ToolTipRole);
        si->setData(item.text, Qt::UserRole);
        model->appendRow(si);
    }
    if (!completer) {
        completer = new QCompleter(doc);
        completer->setObjectName("beqt/completer");
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setWidget(doc->innerEdit());
        completer->setModel(model);
    }
    BAutoCompletionHelper *helper = doc->findChild<BAutoCompletionHelper *>("beqt/completion_helper");
    if (!helper) {
        helper = new BAutoCompletionHelper(doc, model);
        helper->setObjectName("beqt/completion_helper");
        completer->popup()->installEventFilter(helper);
        QObject::connect(completer, SIGNAL(activated(QModelIndex)), helper, SLOT(completerActivated(QModelIndex)));
    }
    helper->textCursor = cursor;
    QRect cursorRect = doc->cursorRect();
    QScrollBar *vsbar = completer->popup()->verticalScrollBar();
    cursorRect.setWidth(completer->popup()->sizeHintForColumn(0) + (vsbar ? vsbar->sizeHint().width() : 0));
    completer->popup()->setCurrentIndex(model->index(0, 0));
    completer->complete(cursorRect);
}

void BAbstractFileType::showToolTip(BAbstractCodeEditorDocument *doc, QTextCursor cursor, const QPoint &globalPos)
{
    QString text = createToolTipText(doc, cursor);
    if (text.isEmpty())
        return QToolTip::hideText();
    QToolTip::showText(globalPos, text);
}
