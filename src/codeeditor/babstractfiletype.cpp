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
protected:
    void highlightBlock(const QString &text);
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

/*============================== Protected methods =========================*/

void BDefaultFileType::highlightBlock(const QString &)
{
    //
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

/*============================== Protected methods =========================*/

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
