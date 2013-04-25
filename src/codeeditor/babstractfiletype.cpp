class QColor;
class QFont;

#include "babstractfiletype.h"
#include "bcodeedit.h"
#include "btextblockuserdata.h"
#include "bcodeedit_p.h"

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
    BDefaultFileType();
    ~BDefaultFileType();
public:
    QString id() const;
    QString name() const;
    QString description() const;
    QStringList suffixes() const;
    bool matchesFileName(const QString &fileName) const;
    QList<BCodeEdit::BracketPair> brackets() const;
protected:
    void highlightBlock(const QString &text);
};

/*============================================================================
================================ BAbstractFileTypePrivate ====================
============================================================================*/

class BAbstractFileTypePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractFileType)
public:
    explicit BAbstractFileTypePrivate(BAbstractFileType *q);
    ~BAbstractFileTypePrivate();
public:
    void init();
public:
    BSyntaxHighlighter *highlighter;
private:
    Q_DISABLE_COPY(BAbstractFileTypePrivate)
};

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

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

QList<BCodeEdit::BracketPair> BDefaultFileType::brackets() const
{
    return QList<BCodeEdit::BracketPair>();
}

/*============================== Protected methods =========================*/

void BDefaultFileType::highlightBlock(const QString &)
{
    //
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

BAbstractFileType *BAbstractFileType::defaultFileType()
{
    return new BDefaultFileType;
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

BCodeEdit::BracketPair BAbstractFileType::createBracketPair(const QString &op, const QString &cl, const QString &esc)
{
    BCodeEdit::BracketPair bp;
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

/*============================== Private methods ===========================*/

void BAbstractFileType::setCurrentHighlighter(BSyntaxHighlighter *highlighter)
{
    d_func()->highlighter = highlighter;
}

BSyntaxHighlighter *BAbstractFileType::currentHighlighter() const
{
    return d_func()->highlighter;
}
