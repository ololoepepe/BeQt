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

class QColor;
class QFont;

#include "babstractfiletype.h"
#include "babstractfiletype_p.h"
#include "bcodeedit.h"
#include "btextblockuserdata.h"
#include "babstractcodeeditordocument_p.h"

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QList>
#include <QTextBlockUserData>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QPoint>
#include <QMenu>

#include <BeQtCore/private/bbase_p.h>

#include <QDebug>

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

class BDefaultFileType : public BAbstractFileType
{
    Q_DECLARE_TR_FUNCTIONS(BDefaultFileType)
public:
    static BDefaultFileType *instance();
public:
    BDefaultFileType();
    ~BDefaultFileType();
public:
    QString id() const;
    QString name() const;
    QString description() const;
    QStringList suffixes() const;
    bool matchesFileName(const QString &fileName) const;
    BracketPairList brackets() const;
protected:
    static BDefaultFileType *_m_self;
};

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

/*============================== Static public methods =====================*/

BDefaultFileType *BDefaultFileType::instance()
{
    if (!_m_self)
        _m_self = new BDefaultFileType;
    return _m_self;
}

/*============================== Public constructors =======================*/

BDefaultFileType::BDefaultFileType()
{
    //
}

BDefaultFileType::~BDefaultFileType()
{
    //
}

/*============================== Public methods ============================*/

QString BDefaultFileType::id() const
{
    return "Text";
}

QString BDefaultFileType::name() const
{
    return tr("Text", "name");
}

QString BDefaultFileType::description() const
{
    return tr("All files", "description");
}

QStringList BDefaultFileType::suffixes() const
{
    return QStringList() << "*";
}

bool BDefaultFileType::matchesFileName(const QString &) const
{
    return true;
}

BAbstractFileType::BracketPairList BDefaultFileType::brackets() const
{
    return BracketPairList();
}

/*============================== Static protected variables ================*/

BDefaultFileType *BDefaultFileType::_m_self = 0;

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

BAbstractFileType *BAbstractFileType::defaultFileType()
{
    return BDefaultFileType::instance();
}

QString BAbstractFileType::defaultFileTypeId()
{
    return defaultFileType()->id();
}

bool BAbstractFileType::areEqual(const BracketPair &bp1, const BracketPair &bp2)
{
    return bp1.opening == bp2.opening && bp1.closing == bp2.closing && bp1.escape == bp2.escape;
}

bool BAbstractFileType::areEqual(const BracketPairList &l1, const BracketPairList &l2)
{
    if (l1.size() != l2.size())
        return false;
    for (int i = 0; i < l1.size(); ++i)
        if (!areEqual(l1.at(i), l2.at(i)))
            return false;
    return true;
}

/*============================== Public methods ============================*/

BAbstractCodeEditorDocument *BAbstractFileType::currentDocument() const
{
    if (!d_func()->highlighter)
        return 0;
    return d_func()->highlighter->EditorDocument;
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

QString BAbstractFileType::createFileDialogFilter() const
{
    QString desc = description();
    QStringList sl = suffixes();
    if ( desc.isEmpty() || sl.isEmpty() )
        return "";
    QString filter;
    filter += desc + " (";
    if ( sl.contains("*") )
        sl.clear();
    sl.removeDuplicates();
    if ( !sl.isEmpty() )
        for (int i = 0; i < sl.size(); ++i)
            filter += "*." + sl.at(i) + (i < sl.size() - 1 ? " " : "");
    else
        filter += "*";
    filter += ")";
    return filter;
}

/*============================== Static protected methods ==================*/

BAbstractFileType::BracketPair BAbstractFileType::createBracketPair(const QString &op, const QString &cl,
                                                                    const QString &esc)
{
    BracketPair bp;
    bp.opening = op;
    bp.closing = cl;
    bp.escape = esc;
    return bp;
}

BAbstractFileType::AutocompletionItem BAbstractFileType::createAutocompletionItem(const QString &text,
                                                                                  const QString &toolTip, const QIcon &icon)
{
    if (text.isEmpty())
        return AutocompletionItem();
    AutocompletionItem item;
    item.text = text;
    item.toolTip = toolTip;
    item.icon = icon;
    return item;
}

void BAbstractFileType::setBlockSkipIntervals(QTextBlock block, const QList<BTextBlockUserData::SkipInterval> &list)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    if (!ud) {
        ud = new BTextBlockUserData(list);
        block.setUserData(ud);
    }
    ud->setSkipIntervals(list);
}

void BAbstractFileType::addBlockSkipInterval(QTextBlock block, const BTextBlockUserData::SkipInterval &si)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>(block.userData());
    QList<BTextBlockUserData::SkipInterval> list;
    list << si;
    if (ud) {
        list << ud->skipIntervals();
        ud->setSkipIntervals(list);
    } else {
        ud = new BTextBlockUserData(list);
        block.setUserData(ud);
    }

}

void BAbstractFileType::addBlockSkipInterval(QTextBlock block, int start, int end)
{
    BTextBlockUserData::SkipInterval si;
    si.start = start;
    si.end = (end >= 0) ? end : (block.text().length() - 1);
    addBlockSkipInterval(block, si);
}

/*============================== Protected methods =========================*/

void BAbstractFileType::highlightBlock(const QString &)
{
    //
}

void BAbstractFileType::showAutocompletionMenu(BAbstractCodeEditorDocument *doc, QTextBlock block, int posInBlock,
                                               const QPoint &globalPos)
{
    QList<AutocompletionItem> list = createAutocompletionItemList(doc, block, posInBlock);
    if (list.isEmpty())
        return;
    QMenu *mnu = new QMenu;
    foreach (const AutocompletionItem &aci, list) {
        if (aci.text.isEmpty())
            continue;
        mnu->addAction(aci.icon, aci.text)->setToolTip(aci.toolTip);
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
    QString text = act->text();
    delete mnu;
    QString btext = block.text();
    int start = block.position() + posInBlock - 1;
    int end = start + 1;
    while (start > 0 && start < btext.length() && btext.at(start) != ' ')
        --start;
    doc->selectText(start + 1, end);
    doc->insertText(text);
}

QList<BAbstractFileType::AutocompletionItem> BAbstractFileType::createAutocompletionItemList(
        BAbstractCodeEditorDocument *, QTextBlock, int)
{
    return QList<AutocompletionItem>();
}

QTextBlock BAbstractFileType::currentBlock() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlock() : QTextBlock();
}

int BAbstractFileType::currentBlockState() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlockState() : -1;
}

BTextBlockUserData *BAbstractFileType::currentBlockUserData() const
{
    return d_func()->highlighter ? d_func()->highlighter->currentBlockUserData() : 0;
}

QTextCharFormat BAbstractFileType::format(int position) const
{
    return d_func()->highlighter ? d_func()->highlighter->format(position) : QTextCharFormat();
}

int BAbstractFileType::previousBlockState() const
{
    return d_func()->highlighter ? d_func()->highlighter->previousBlockState() : -1;
}

void BAbstractFileType::setCurrentBlockState(int newState)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setCurrentBlockState(newState);
}

void BAbstractFileType::setCurrentBlockUserData(BTextBlockUserData *data)
{
    if (!d_func()->highlighter)
        return;
    d_func()->highlighter->setCurrentBlockUserData(data);
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
