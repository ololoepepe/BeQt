#include "bsyntax.h"
#include "private/bsyntaxelement.h"
#include "private/bmultilinesyntaxelement.h"

#include <QString>
#include <QRegExp>
#include <QList>
#include <QChar>
#include <QVector>
#include <QTextCharFormat>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QBrush>
#include <QColor>
#include <QTextCursor>
#include <QTextBlock>
#include <QMap>
#include <QSharedData>
#include <QFileInfo>

#include <QDebug>

BSyntaxData::BSyntaxData()
{
    bracketsSkipHighlighted = false;
}

BSyntaxData::~BSyntaxData()
{
    for (int i = 0; i < elements.size(); ++i)
        delete elements.at(i);
    elements.clear();
}

//

const QColor BSyntax::BracketsColor = QColor("yellow");
const QColor BSyntax::BracketsErrorColor = QColor("hotpink");

//

void BSyntax::removeSpaces(QString &text)
{
    static QRegExp rx("\\s+$");
    text.remove(rx);
}

void setFormat(QVector<QTextCharFormat> &formats, int start, int length, const QTextCharFormat &format)
{
    if ( start < 0 || length < 1 || formats.size() < (start + length) )
        return;
    for (int i = start; i < start + length; ++i)
        formats[i] = format;
}

//

BSyntax::BSyntax() : _m_d(new BSyntaxData)
{
    _m_d->type = "Text";
}

BSyntax::BSyntax(const QString &fileName) : _m_d(new BSyntaxData)
{
    loadSyntax(fileName);
}

BSyntax::BSyntax(const BSyntax &other) : _m_d(other._m_d)
{
}

BSyntax::~BSyntax()
{
    //
}

//

void BSyntax::loadSyntax(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return;
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return;
    QDomDocument doc("BTEXTEDITOR_SYNTAX");
    if ( !doc.setContent(&f) )
        return f.close();
    f.close();
    QDomElement root = doc.documentElement();
    if ( root.isNull() || !BSyntaxElement::equals(root.tagName(), "syntax") )
        return;
    for (int i = 0; i < _m_d->elements.size(); ++i)
        delete _m_d->elements.at(i);
    _m_d->elements.clear();
    _m_loadType(root);
    _m_loadPattern(root);
    _m_loadBrackets(root);
    _m_loadElements(root);
}

bool BSyntax::isValid() const
{
    return !_m_d->type.isEmpty();
}

bool BSyntax::matchesFileName(const QString &fileName) const
{
    return !_m_d->pattern.pattern().isEmpty() && QFileInfo(fileName).fileName().indexOf(_m_d->pattern) >= 0;
}

const QString &BSyntax::type() const
{
    return _m_d->type;
}

BSyntax::BlockProcessingResult BSyntax::processBlock(QString text, int previousState) const
{
    BlockProcessingResult result;
    result.blockData = 0;
    result.currentState = 0;
    removeSpaces(text);
    if ( text.isEmpty() )
    {
        result.currentState = previousState;
        return result;
    }
    QVector<BracketInfo> brackets = _m_brackets(text);
    if ( !brackets.isEmpty() )
    {
        result.blockData = new BlockData;
        result.blockData->brackets = brackets;
    }
    QVector<QTextCharFormat> &formats = result.formats;
    formats.resize( text.length() );
    for (int i = 0; i < _m_d->elements.size(); ++i)
    {
        BSyntaxElement *el = _m_d->elements.at(i);
        if ( el->isMultiline() )
        {
            BMultilineSyntaxElement *mel = static_cast<BMultilineSyntaxElement *>(el);
            if (!mel)
                continue;
            QRegExp rx( mel->pattern(), mel->caseSensitivity(), mel->patternSyntax() );
            const QString &op = mel->opening();
            const QString &cl = mel->closing();
            int sind = 0;
            bool b = (1 != previousState);
            const QChar &pc = mel->preventing();
            if (b)
                sind = _m_indexOf(text, rx, pc, op);
            while (sind >= 0)
            {
                int eind = _m_indexOf( text, rx, pc, cl, sind + (b ? rx.matchedLength() : 0) );
                b = true;
                int len;
                if (-1 == eind)
                {
                    result.currentState = 1;
                    len = text.length() - sind;
                }
                else
                {
                    len = eind - sind + rx.matchedLength();
                }
                setFormat( formats, sind, len, mel->format() );
                sind = _m_indexOf(text, rx, pc, op, sind + len);
            }
        }
        else
        {
            QRegExp rx( el->pattern(), el->caseSensitivity(), el->patternSyntax() );
            const QChar &pc = el->preventing();
            int ind = text.indexOf(rx);
            while (ind >= 0)
            {
                if (pc.isNull() || !ind || text.at(ind - 1) != pc)
                {
                    int l = rx.matchedLength();
                    setFormat( formats, ind, l, el->format() );
                    ind = text.indexOf(rx, ind + l);
                }
                else
                {
                    ind = text.indexOf(rx, ind + 1);
                }
            }
        }
    }
    if (_m_d->bracketsSkipHighlighted && result.blockData)
    {
        QVector<BracketInfo> &brts = result.blockData->brackets;
        for (int i = brts.size() - 1; i >= 0; --i)
            if ( QTextCharFormat() != formats.at(brts.at(i).position) )
                brts.remove(i);
        if ( brts.isEmpty() )
        {
            delete result.blockData;
            result.blockData = 0;
        }
    }
    return result;
}

BSyntax::CursorProcessingResult BSyntax::processCursor(const QTextCursor &cursor) const
{
    CursorProcessingResult result;
    if ( cursor.isNull() )
        return result;
    QTextBlock tb = cursor.block();
    if ( !tb.isValid() )
        return result;
    BlockData *data = static_cast<BlockData *>( tb.userData() );
    if (!data)
        return result;
    QString text = tb.text();
    //removeSpaces(text); //hell knows how, but this leads to issues
    if ( text.isEmpty() )
        return result;
    int posb = cursor.positionInBlock();
    if ( posb >= text.length() )
        return result;
    int posop = posb;
    int poscl = posb - 1;
    bool op = _m_testBracket(text, posop, true);
    bool cl = _m_testBracket(text, poscl, false);
    if (!op && !cl)
        return result;
    if (_m_d->bracketsSkipHighlighted)
    {
        bool b = false;
        for (int i = 0; i < data->brackets.size(); ++i)
        {
            if ( (op && data->brackets.at(i).position == posop) || (cl && data->brackets.at(i).position == poscl) )
            {
                b = true;
                break;
            }
        }
        if (!b)
            return result;
    }
    if (op)
    {
        BracketInfo bi = _m_findBracketPair(tb, posop, true);
        if (bi.position >= 0)
        {
            int ind = _m_d->opBr.indexOf( text.at(posop) );
            bool b = (_m_d->clBr.at(ind) != bi.bracket);
            result.positions.insert(posop + tb.position(), b);
            result.positions.insert(bi.position, b);
        }
        else
        {
            result.positions.insert(posop + tb.position(), true);
        }
    }
    if (cl)
    {
        BracketInfo bi = _m_findBracketPair(tb, poscl, false);
        if (bi.position >= 0)
        {
            int ind = _m_d->clBr.indexOf( text.at(poscl) );
            bool b = (_m_d->opBr.at(ind) != bi.bracket);
            result.positions.insert(bi.position, b);
            result.positions.insert(poscl + tb.position(), b);
        }
        else
        {
            result.positions.insert(poscl + tb.position(), true);
        }
    }
    return result;
}

//

void BSyntax::_m_loadType(const QDomElement &root)
{
    if ( root.isNull() )
        return;
    QDomElement element = root.firstChildElement("type");
    if ( element.isNull() )
        return;
    _m_d->type = element.text();
}

void BSyntax::_m_loadPattern(const QDomElement &root)
{
    if ( root.isNull() )
        return;
    QDomElement element = root.firstChildElement("pattern");
    if ( element.isNull() )
        return;
    _m_d->pattern.setPattern( element.text() );
    _m_d->pattern.setPatternSyntax( BSyntaxElement::syntax(element) );
}

void BSyntax::_m_loadBrackets(const QDomElement &root)
{
    if ( root.isNull() )
        return;
    _m_d->opBr.clear();
    _m_d->clBr.clear();
    QDomElement element = root.firstChildElement("brackets");
    if ( element.isNull() )
    {
        _m_d->opBr << '(' << '[' << '{';
        _m_d->clBr << ')' << ']' << '}';
        return;
    }
    QDomElement el = element.firstChildElement("escape");
    if ( !el.isNull() )
    {
        QString text = el.text();
        if (text.size() == 1)
            _m_d->bracketsEscape = text.at(0);
    }
    _m_d->bracketsSkipHighlighted = !element.firstChildElement("skip_highlighted").isNull();
    if ( element.firstChildElement("noround").isNull() )
    {
        _m_d->opBr << '(';
        _m_d->clBr << ')';
    }
    if ( element.firstChildElement("nosquare").isNull() )
    {
        _m_d->opBr << '[';
        _m_d->clBr << ']';
    }
    if ( element.firstChildElement("nocurly").isNull() )
    {
        _m_d->opBr << '{';
        _m_d->clBr << '}';
    }
}

void BSyntax::_m_loadElements(const QDomElement &root)
{
    if ( root.isNull() )
        return;
    QDomElement el = root.firstChildElement("element");
    while ( !el.isNull() )
    {
        BSyntaxElement *sel = 0;
        if ( BSyntaxElement::equals(el.attribute("multiline"), "true") )
            sel = new BMultilineSyntaxElement(el);
        else
            sel = new BSyntaxElement(el);
        if ( sel->isValid() )
            _m_d->elements << sel;
        else
            delete sel;
        el = el.nextSiblingElement();
    }
}

QVector<BSyntax::BracketInfo> BSyntax::_m_brackets(const QString &text) const
{
    QVector<BracketInfo> brackets;
    if ( text.isEmpty() )
        return brackets;
    for (int i = 0; i < text.size(); ++i)
    {
        if ( _m_testBracket(text, i) )
        {
            BracketInfo bi;
            bi.bracket = text.at(i);
            bi.position = i;
            brackets.append(bi);
        }
    }
    return brackets;
}

bool BSyntax::_m_testBracket(const QString &text, int pos) const
{
    if ( text.isEmpty() || pos < 0 || pos >= text.length() )
        return false;
    const QChar &c = text.at(pos);
    if ( !_m_d->opBr.contains(c) && !_m_d->clBr.contains(c) )
        return false;
    if (!_m_d->bracketsEscape.isNull() && pos > 0 && text.at(pos - 1) == _m_d->bracketsEscape)
        return false;
    return true;
}

bool BSyntax::_m_testBracket(const QString &text, int pos, bool opening) const
{
    if ( text.isEmpty() || pos < 0 || pos >= text.length() )
        return false;
    const QChar &c = text.at(pos);
    if ( ( opening && !_m_d->opBr.contains(c) ) || ( !opening && !_m_d->clBr.contains(c) ) )
        return false;
    if (!_m_d->bracketsEscape.isNull() && pos > 0 && text.at(pos - 1) == _m_d->bracketsEscape)
        return false;
    return true;
}

BSyntax::BracketInfo BSyntax::_m_findBracketPair(const QTextBlock &block, int pos, bool opening) const
{
    BracketInfo bi;
    bi.position = -1;
    if ( !block.isValid() || pos < 0 || pos >= block.length() )
        return bi;
    if ( !_m_testBracket(block.text(), pos, opening) )
        return bi;
    int depth = 0;
    bool b = true;
    QTextBlock tb = block;
    while ( tb.isValid() )
    {
        BlockData *data = static_cast<BlockData *>( tb.userData() );
        if (!data)
        {
            tb = opening ? tb.next() : tb.previous();
            b = false;
            continue;
        }
        QString text = tb.text();
        int lb = opening ? 0 : data->brackets.size() - 1;
        int ub = opening ? data->brackets.size() : -1;
        int m = opening ? 1 : -1;
        for (int i = lb; i != ub; i += m)
        {
            int position = data->brackets.at(i).position;
            if ( b && ( (opening && position <= pos) || (!opening && position >= pos) ) )
                continue;
            if (_m_testBracket(text, position, !opening) && !depth--)
            {
                bi.bracket = data->brackets.at(i).bracket;
                bi.position = tb.position() + position;
                return bi;
            }
            else if ( _m_testBracket(text, position, opening) )
            {
                ++depth;
            }
        }
        b = false;
        tb = opening ? tb.next() : tb.previous();
    }
    return bi;
}

int BSyntax::_m_indexOf(const QString &text, QRegExp &rx, const QChar &prev, const QString &pattern, int from) const
{
    if ( text.isEmpty() )
        return -1;
    int ind = text.indexOf(rx, from);
    while (-1 != ind)
    {
        int len = rx.matchedLength();
        if ( (prev.isNull() || !ind || text.at(ind - 1) != prev) &&
             (pattern.isEmpty() || text.mid(ind, len) == pattern) )
            return ind;
        ind = text.indexOf(rx, ind + len);
    }
    return -1;
}
