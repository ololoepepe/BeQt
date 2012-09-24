#include "btexteditordocument.h"
#include "bplaintextedit.h"
#include "bsyntaxhighlighter.h"
#include "bsplittedlinesdialog.h"
#include "bmacrorecorder.h"
#include "../bsyntax.h"
#include "../bkeyboardlayoutmap.h"

#include "../../bcore/bcore.h"

#include <QObject>
#include <QString>
#include <QWidget>
#include <QPlainTextEdit>
#include <QPalette>
#include <QColor>
#include <QTextCharFormat>
#include <QMenu>
#include <QAction>
#include <QRegExp>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QChar>
#include <QFont>
#include <QTextCursor>
#include <QTextBlock>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QChar>
#include <QApplication>
#include <QClipboard>
#include <QTextDocument>
#include <QFileInfo>
#include <QStringList>
#include <QWidget>
#include <QTextOption>
#include <QPoint>
#include <QKeySequence>
#include <QMap>
#include <QCharRef>
#include <QPointer>
#include <QTextLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QFontInfo>

#include <QDebug>

typedef QTextEdit::ExtraSelection _m_ExtraSelection;
typedef QList<_m_ExtraSelection> _m_ExtraSelectionList;

struct _m_ProcessLoadedTextResult
{
    QString text;
    QList<int> lineIndexes;
};

typedef QFuture<_m_ProcessLoadedTextResult> _m_Future;
typedef QFutureWatcher<_m_ProcessLoadedTextResult> _m_FutureWatcher;

const int BookmarkCountMax = 10;
const QList<QChar> _m_UnprintedList = QList<QChar>() << QChar(1) << QChar(2) << QChar(3) << QChar(4) << QChar(5)
    << QChar(6) << QChar(7) << QChar(8) << QChar(9) << QChar(11) << QChar(12) << QChar(13) << QChar(14) << QChar(15)
    << QChar(16) << QChar(17) << QChar(18) << QChar(19) << QChar(20) << QChar(21) << QChar(22) << QChar(23)
    << QChar(24) << QChar(25) << QChar(26) << QChar(27) << QChar(28) << QChar(29) << QChar(30) << QChar(31);

//

void removeSpaces(QString &text)
{
    //static QRegExp rx("\\s+(?=\\n|$)");
    static QRegExp rx("\\s+$");
    QStringList sl = text.split('\n');
    for (int i = 0; i < sl.size(); ++i)
        sl[i].remove(rx);
    text = sl.join("\n");
    //text.remove(rx);
}

void appendSpaces(QString &line, int lineLength)
{
    int len = line.length();
    if (len < lineLength)
        line.append( QString().fill(' ', lineLength - len) );
}

void replaceTabs(QString &line, int tabWidth)
{
    int i = 0;
    while ( i < line.length() )
    {
        if (line.at(i) == '\t')
        {
            int spcount = tabWidth - (i < tabWidth ? i : i % tabWidth);
            line.replace( i, 1, QString().fill(' ', spcount ? spcount : tabWidth) );
            i += spcount;
        }
        else
        {
            ++i;
        }
    }
}

QStringList processLoadedTextLine(QString line, int lineLength, int tabWidth)
{
    QStringList sl;
    replaceTabs(line, tabWidth);
    int len = line.length();
    if (len > lineLength)
    {
        while (line.length() > lineLength)
        {
            int k = lineLength;
            while (k > 1 && line.at(k) != ' ')
                --k;
            int l = k > 1 ? k : lineLength;
            QString nline = line.left(l);;
            appendSpaces(nline, lineLength);
            sl << nline;
            line.remove( 0, l + (k > 1 ? 1 : 0) );
        }
        if ( !line.isEmpty() )
        {
            appendSpaces(line, lineLength);
            sl << line;
        }
    }
    else
    {
        appendSpaces(line, lineLength);
        sl << line;
    }
    return sl;
}

_m_ProcessLoadedTextResult processLoadedText(QString text, int lineLength, int tabWidth)
{
    _m_ProcessLoadedTextResult result;
    text.remove('\r');
    for (int i = 0; i < _m_UnprintedList.size(); ++i)
        text.remove( _m_UnprintedList.at(i) );
    QStringList sl = text.split('\n');
    QStringList sln;
    for (int i = 0; i < sl.size(); ++i)
    {
        QStringList lines = processLoadedTextLine(sl.at(i), lineLength, tabWidth);
        if (lines.size() > 1)
            for (int j = lines.size() - 1; j >= 0; --j)
                result.lineIndexes.prepend(sln.size() + j);
        sln << lines;
    }
    result.text = sln.join("\n");
    return result;
}

//

const QStringList BTextEditorDocument::EncodingsValid = QStringList() << "UTF-16" << "UTF-8" << "ISO 8859-13"
    << "ISO 8859-4" << "Windows-1257" << "ISO 8859-6" << "KOI8-R" << "Windows-1251" << "KOI8-U" << "ISO 8859-16"
    << "ISO 8859-2" << "Windows-1250" << "ISO 8859-7" << "Windows-1253" << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
    << "Apple Roman" << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10" << "ISO 8859-3" << "Windows-1258" << "Big5"
    << "Big5-HKSCS" << "GB18030-0" << "EUC-KR" << "JOHAB" << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS" << "TIS-620"
    << "ISO 8859-9" << "Windows-1254" << "ISO 8859-6" << "Windows-1256" << "Windows-1255" << "ISO 8859-8";
const QString BTextEditorDocument::EncodingDef = "UTF-8";
const int BTextEditorDocument::FontPointSizeMin = 6;
const int BTextEditorDocument::FontPointSizeDef = 10;
const int BTextEditorDocument::FontPointSizeMax = 20;
const int BTextEditorDocument::LineLengthMin = 80;
const int BTextEditorDocument::LineLengthDef = 300;
const int BTextEditorDocument::LineLengthMax = 500;
const int BTextEditorDocument::TabWidthDef = 4;
const QList<int> BTextEditorDocument::TabWidthsValid = QList<int>() << 2 << 4 << 8;
const bool BTextEditorDocument::BlockModeDef = false;
#if defined(Q_OS_MAC)
const QString BTextEditorDocument::FontFamilyDef = "Monaco";
#elif defined(Q_OS_UNIX)
const QString BTextEditorDocument::FontFamilyDef = "DejaVu Sans Mono";
#elif defined(Q_OS_WIN)
const QString BTextEditorDocument::FontFamilyDef = "Courier";
#endif

//

bool BTextEditorDocument::checkEncoding(const QString &codecName)
{
    return !codecName.isEmpty() && EncodingsValid.contains(codecName);
}

bool BTextEditorDocument::checkFontFamily(const QString &family)
{
    QFont fnt(family);
    QFontInfo fnti(fnt);
    return fnti.fixedPitch();
}

bool BTextEditorDocument::checkFontPointSize(int pointSize)
{
    return pointSize >= FontPointSizeMin && pointSize <= FontPointSizeMax;
}

bool BTextEditorDocument::checkLineLength(int length)
{
    return length >= LineLengthMin && length <= LineLengthMax;
}

bool BTextEditorDocument::checkTabWidth(int width)
{
    return TabWidthsValid.contains(width);
}

//

BTextEditorDocument::BTextEditorDocument(const QString &fileName, const QString &codecName, QObject *parent) :
    QObject(parent)
{
    _m_fileName = fileName;
    _m_lineLength = LineLengthDef;
    _m_tabWidth = TabWidthDef;
    _m_codecName = checkEncoding(codecName) ? codecName : EncodingDef;
    _m_initWidget();
    _m_initMenu();
    _m_blockMode = BlockModeDef;
    _m_recorder = 0;
    setFontFamily(FontFamilyDef);
    setFontPointSize(FontPointSizeDef);
    _m_retranslateUi();
    connect( BCore::instance(), SIGNAL( localeChanged() ), this, SLOT( _m_retranslateUi() ) );
}

BTextEditorDocument::~BTextEditorDocument()
{
    if (_m_edit)
        _m_edit->deleteLater();
}

//

bool BTextEditorDocument::eventFilter(QObject *object, QEvent *event)
{
    if (!event)
        return QObject::eventFilter(object, event);
    switch ( event->type() )
    {
    case QEvent::KeyPress:
        return _m_handleKeyPressEvent( object, static_cast<QKeyEvent *>(event) );
    case QEvent::MouseButtonPress:
        return _m_handleMousePressEvent( object, static_cast<QMouseEvent *>(event) );
    case QEvent::MouseButtonDblClick:
        return _m_handleMouseDoubleClickEvent( object, static_cast<QMouseEvent *>(event) );
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void BTextEditorDocument::setMacroRecorder(BMacroRecorder *recorder)
{
    _m_recorder = recorder;
}

void BTextEditorDocument::setReadOnly(bool readOnly)
{
    _m_edit->setReadOnly(readOnly);
    _m_cutAvailableChanged( !_m_edit->isReadOnly() && _m_edit->textCursor().hasSelection() );
    _m_undoAvailableChanged( !_m_edit->isReadOnly() && _m_edit->document()->isUndoAvailable() );
    _m_redoAvailableChanged( !_m_edit->isReadOnly() && _m_edit->document()->isRedoAvailable() );
    _m_modificationChanged( !_m_edit->isReadOnly() && _m_edit->document()->isModified() );
    setClipboardHasText( !QApplication::clipboard()->text().isEmpty() );
}

void BTextEditorDocument::setFontFamily(const QString &family)
{
    if ( !checkFontFamily(family) )
        return;
    QFont fnt = _m_edit->font();
    //fnt.setStyleHint(QFont::TypeWriter);
    fnt.setFamily(family);
    _m_edit->setFont(fnt);
}

void BTextEditorDocument::setFontPointSize(int pointSize)
{
    if ( !checkFontPointSize(pointSize) )
        return;
    QFont fnt = _m_edit->font();
    fnt.setPointSize(pointSize);
    _m_edit->setFont(fnt);
}

void BTextEditorDocument::setBlockMode(bool enabled)
{
    if (enabled == _m_blockMode)
        return;
    _m_blockMode = enabled;
    _m_editSelectionChanged();
}

void BTextEditorDocument::setSyntax(const BSyntax &syntax)
{
    _m_syntax = syntax;
    _m_highlighter->setSyntax(_m_syntax);
    _m_highlighter->rehighlight();
    _m_highlightBrackets();
}

void BTextEditorDocument::setLineLength(int length)
{
    if (!checkLineLength(length) || length == _m_lineLength)
        return;
    _m_lineLength = length;
    QString text = _m_edit->toPlainText();
    QStringList sl = text.split('\n');
    QList<int> lineIndexes;
    for (int i = sl.size() - 1; i >= 0; --i)
    {
        QString &line = sl[i];
        removeSpaces(line);
        int len = line.length();
        if (len > _m_lineLength)
        {
            QStringList lines;
            while (line.length() > _m_lineLength)
            {
                lines << line.left(_m_lineLength);
                line.remove(0, _m_lineLength);
            }
            removeSpaces(line);
            if ( !line.isEmpty() )
                lines << line;
            int llen = lines.last().length();
            if (llen < _m_lineLength)
                lines.last().append( QString().fill(' ', _m_lineLength - llen) );
            sl.removeAt(i);
            QList<int > indl;
            for (int j = 0; j < lines.size(); ++j)
            {
                sl.insert( i + j, lines.at(j) );
                indl << i + j;
            }
            for (int i = indl.size() - 1; i >= 0; --i)
                lineIndexes.prepend( indl.at(i) );
        }
        else
        {
            int llen = line.length();
            if (llen < _m_lineLength)
                line.append( QString().fill(' ', _m_lineLength - llen) );
        }
    }
    bool pm = isModified();
    _m_edit->setPlainText( sl.join("\n") );
    if ( !lineIndexes.isEmpty() )
    {
        _m_edit->document()->setModified(true);
        BSplittedLinesDialog *sld = new BSplittedLinesDialog(lineIndexes, _m_edit);
        sld->setAttribute(Qt::WA_DeleteOnClose, true);
        connect( sld, SIGNAL( gotoLine(int, bool) ), this, SLOT( _m_gotoLine(int, bool) ) );
        sld->show();
    }
    _m_edit->document()->setModified( pm || isModified() );
}

void BTextEditorDocument::setTabWidth(int width)
{
    if (!checkTabWidth(width) || width == _m_tabWidth)
        return;
    _m_tabWidth = width;
}

void BTextEditorDocument::setClipboardHasText(bool b)
{
    _m_actPaste->setEnabled(!_m_edit->isReadOnly() && b);
}

const QString &BTextEditorDocument::fileName() const
{
    return _m_fileName;
}

const QString &BTextEditorDocument::codecName() const
{
    return _m_codecName;
}

bool BTextEditorDocument::isReadOnly() const
{
    return _m_edit->isReadOnly();
}

bool BTextEditorDocument::isCutAvailable() const
{
    return !_m_edit->isReadOnly() && _m_edit->textCursor().hasSelection();
}

bool BTextEditorDocument::isCopyAvailable() const
{
    return _m_edit->textCursor().hasSelection();
}

bool BTextEditorDocument::isPasteAvailable() const
{
    return _m_actPaste->isEnabled();
}

bool BTextEditorDocument::isUndoAvailable() const
{
    return !_m_edit->isReadOnly() && _m_edit->document()->isUndoAvailable();
}

bool BTextEditorDocument::isRedoAvailable() const
{
    return !_m_edit->isReadOnly() && _m_edit->document()->isRedoAvailable();
}

bool BTextEditorDocument::isModified() const
{
    return !_m_edit->isReadOnly() && _m_edit->document()->isModified();
}

bool BTextEditorDocument::hasBookmarks() const
{
    return !_m_bookmarks.isEmpty();
}

bool BTextEditorDocument::hasSelection() const
{
    return _m_edit->textCursor().hasSelection();
}

const BSyntax &BTextEditorDocument::syntax() const
{
    return _m_syntax;
}

BTextEditorDocument::CursorPosition BTextEditorDocument::cursorPosition() const
{
    CursorPosition cp;
    QTextCursor tc = _m_edit->textCursor();
    cp.row = tc.blockNumber();
    cp.column = tc.positionInBlock();
    return cp;
}

QPlainTextEdit *BTextEditorDocument::editWidget() const
{
    return _m_edit.data();
}

QString BTextEditorDocument::text() const
{
    QString text = _m_edit->toPlainText();
    text.replace(QChar::ParagraphSeparator, '\n');
    removeSpaces(text);
    return text;
}

QString BTextEditorDocument::selectedText() const
{
    _m_ExtraSelectionList esl = _m_edit->extraSelections();
    if ( esl.isEmpty() )
        return QString();
    QString text;
    if (_m_blockMode)
    {
        for (int i = 0; i < esl.size(); ++i)
        {
            text += esl.at(i).cursor.selectedText();
            if (i < esl.size() - 1)
                text += '\n';
        }
    }
    else
    {
        text = esl.first().cursor.selectedText().replace(QChar::ParagraphSeparator, '\n');
        if ( text.contains('\n') )
            removeSpaces(text);
    }
    return text;
}

bool BTextEditorDocument::open(const QString &fileName, const QString &codecName)
{
    return _m_setFileName(fileName) ? reopen(codecName) : false;
}

bool BTextEditorDocument::reopen(const QString &codecName)
{
    QFile f(_m_fileName);
    if ( !f.open(QFile::ReadOnly) )
        return false;
    QTextStream in(&f);
    if ( !codecName.isEmpty() )
        _m_codecName = codecName;
    in.setCodec( _m_codecName.toAscii() );
    QString text = in.readAll();
    f.close();
    _m_Future fut = QtConcurrent::run(processLoadedText, text, _m_lineLength, _m_tabWidth);
    _m_FutureWatcher *futw = new _m_FutureWatcher(this);
    futw->setFuture(fut);
    connect( futw, SIGNAL( finished() ), this, SLOT( _m_futureWatcherFinished() ) );
    _m_edit->setEnabled(false);
    _m_edit->setPlainText( tr("Loading document, please wait.", "edt text") );
    return true;
}

bool BTextEditorDocument::save(const QString &codecName)
{
    QFile f(_m_fileName);
    if ( !f.open(QFile::WriteOnly) )
        return false;
    QString text = _m_edit->toPlainText();
    _m_processSavedText(text);
    QTextStream out(&f);
    if ( !codecName.isEmpty() )
        _m_codecName = codecName;
    out.setCodec( _m_codecName.toAscii() );
    out << text;
    f.close();
    _m_edit->document()->setModified(false);
    return true;
}

bool BTextEditorDocument::saveAs(const QString &fileName, const QString &codecName)
{
    return _m_setFileName(fileName) ? save(codecName) : false;
}

bool BTextEditorDocument::find(const QString &text, QTextDocument::FindFlags flags, bool cyclic)
{
    if ( text.isEmpty() )
        return false;
    bool b = _m_edit->find(text, flags);
    if (!b && cyclic)
    {
        if (flags & QTextDocument::FindBackward)
            _m_edit->moveCursor(QTextCursor::End);
        else
            _m_edit->moveCursor(QTextCursor::Start);
        b = _m_edit->find(text, flags);
    }
    if (b)
        _m_editSelectionChanged();
    return b;
}

bool BTextEditorDocument::replace(const QString &newText)
{
    if (_m_edit->isReadOnly() || newText.length() > _m_lineLength)
        return false;
    QTextCursor tc = _m_edit->textCursor();
    if ( !tc.hasSelection() )
        return false;
    tc.beginEditBlock();
    _m_deleteSelection();
    if ( !newText.isEmpty() )
        insertText(newText);
    tc.endEditBlock();
    return true;
}

int BTextEditorDocument::replaceInSelection(const QString &text, const QString &newText, Qt::CaseSensitivity cs)
{
    if (_m_edit->isReadOnly() || text.length() > _m_lineLength || newText.length() > _m_lineLength ||
            text.isEmpty() || text == newText)
        return -1;
    return _m_blockMode ? _m_replaceInSelectionBlocks(text, newText, cs) :
                          _m_replaceInSelectionLines(text, newText, cs);
}

int BTextEditorDocument::replaceInDocument(const QString &text, const QString &newText, Qt::CaseSensitivity cs)
{
    QTextCursor tc = _m_edit->textCursor();
    int pos = tc.position();
    QString ptext = _m_edit->toPlainText();
    bool bm = _m_blockMode;
    setBlockMode(false);
    tc.movePosition(QTextCursor::Start);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    int count = replaceInSelection(text, newText, cs);
    setBlockMode(bm);
    if (count > 0)
    {
        int lind = ptext.lastIndexOf(text, -1, cs);
        int llnl = _m_lineLength + 1;
        int llinepos = (lind / llnl) * llnl;
        QString lline = ptext.mid(llinepos, _m_lineLength);
        removeSpaces(lline);
        int llind = lline.lastIndexOf(text, -1, cs);
        QString llinex = lline.left(llind).replace(text, newText, cs);
        lline.replace(0, llind, llinex);
        appendSpaces(lline, _m_lineLength);
        ptext.replace(llinepos, _m_lineLength, lline);
        lind = ptext.lastIndexOf(text, -1, cs);
        int dpos = ptext.length() - lind - newText.length();
        removeSpaces(lline);
        if (lline.length() + ( newText.length() - text.length() ) > _m_lineLength)
        {
            dpos += _m_lineLength + 1;
            int x = lline.lastIndexOf(text, -1, cs) + newText.length();
            if (x > _m_lineLength)
            {
                dpos -= x - _m_lineLength;
                if ( (lline.lastIndexOf(text, -1, cs) + text.length() ) == _m_lineLength )
                    dpos += 1;
            }
        }
        pos = _m_edit->toPlainText().length() - dpos;
    }
    tc.setPosition(pos);
    _m_edit->setTextCursor(tc);
    return count;
}

void BTextEditorDocument::insertText(const QString &text)
{
    if ( _m_edit->isReadOnly() || text.isEmpty() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.beginEditBlock();
    _m_deleteSelection();
    tc = _m_edit->textCursor();
    int posb = tc.positionInBlock();
    QString btext = tc.block().text();
    QString ltext = btext.left(posb);
    QString rtext = btext.right(btext.length() - posb);
    removeSpaces(rtext);
    QString textx = text;
    textx.remove('\r');
    QStringList sl = textx.split('\n');
    for (int i = 0; i < sl.size(); ++i)
    {
        QString &line = sl[i];
        int j = 0;
        while ( j < line.length() )
        {
            if (line.at(j) == '\t')
            {
                int spcount = _m_tabWidth - (j < _m_tabWidth ? j : j % _m_tabWidth);
                line.replace( j, 1, QString().fill(' ', spcount ? spcount : _m_tabWidth) );
                j += spcount;
            }
            else
            {
                ++j;
            }
        }
    }
    bool b = false;
    if ( _m_blockMode && sl.size() > 1 && sl.size() < _m_edit->blockCount() )
    {
        b = true;
        int len = sl.first().length();
        for (int i = 0; i < sl.size(); ++i)
        {
            if (sl.at(i).length() != len)
            {
                b = false;
                break;
            }
        }
    }
    if (b)
    {
        int offset = ltext.length();
        QTextBlock tb = tc.block();
        int  i = 0;
        while ( tb.isValid() && i < sl.size() )
        {
            int bpos = tb.position();
            QString t = tb.text();
            t.insert( offset, sl.at(i) );
            if (t.length() > _m_lineLength)
                t.truncate(_m_lineLength);
            _m_addSpaces(t);
            tc.setPosition(bpos);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText(t);
            tb = tb.next();
            ++i;
        }
        QString spaces;
        spaces.fill(' ', offset);
        while ( i < sl.size() )
        {
            tc.insertBlock();
            QString &t = sl[i];
            t.prepend(spaces);
            if (t.length() > _m_lineLength)
                t.truncate(_m_lineLength);
            _m_addSpaces(t);
            tc.insertText(t);
            ++i;
        }
        tb = tc.block().next();
        if ( !tb.isValid() )
        {
            tc.movePosition(QTextCursor::EndOfBlock);
            tc.insertBlock();
            tc.insertText( QString().fill(' ', _m_lineLength) );
            tc.setPosition(tc.block().position() + offset);
        }
        else
        {
            tc.setPosition(tb.position() + offset);
        }
    }
    else
    {
        sl.first().prepend(ltext);
        sl.last().append(rtext);
        _m_ProcessLoadedTextResult result = processLoadedText(sl.join("\n"), _m_lineLength, _m_tabWidth);
        int boffset = tc.block().blockNumber();
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(result.text);
        int pos = _m_lineLength;
        QString btext = tc.block().text();
        while (pos > 0 && btext.at(pos - 1) == ' ')
            --pos;
        if (pos > 0)
            pos -= rtext.length();
        tc.setPosition(tc.block().position() + pos);
        if ( !result.lineIndexes.isEmpty() )
        {
            for (int i = 0; i < result.lineIndexes.size(); ++i)
                result.lineIndexes[i] += boffset;
            BSplittedLinesDialog *sld = new BSplittedLinesDialog(result.lineIndexes, _m_edit);
            sld->setAttribute(Qt::WA_DeleteOnClose, true);
            connect( sld, SIGNAL( gotoLine(int, bool) ), this, SLOT( _m_gotoLine(int, bool) ) );
            sld->show();
        }
    }
    _m_edit->setTextCursor(tc);
    _m_highlighter->rehighlight();
    tc.endEditBlock();
}

void BTextEditorDocument::setText(const QString &text)
{
    if ( _m_edit->isReadOnly() || text.isEmpty() )
        return;
    _m_Future fut = QtConcurrent::run(processLoadedText, text, _m_lineLength, _m_tabWidth);
    _m_FutureWatcher *futw = new _m_FutureWatcher(this);
    futw->setFuture(fut);
    connect( futw, SIGNAL( finished() ), this, SLOT( _m_futureWatcherFinished() ) );
    _m_edit->setEnabled(false);
    //maybe some message should be displayed
}

void BTextEditorDocument::deselect()
{
    QTextCursor tc = _m_edit->textCursor();
    if ( !tc.hasSelection() )
        return;
    tc.setPosition( tc.selectionEnd() );
    _m_edit->setTextCursor(tc);
}

void BTextEditorDocument::makeBookmark()
{
    QTextCursor tc = _m_edit->textCursor();
    _m_Bookmark bm;
    bm.block = tc.block();
    bm.posInBlock = tc.positionInBlock();
    if ( _m_bookmarks.contains(bm) )
        _m_bookmarks.removeAll(bm);
    _m_bookmarks.prepend(bm);
    while (_m_bookmarks.size() > BookmarkCountMax)
        _m_bookmarks.removeLast();
    emit hasBookmarkChanged( hasBookmarks() );
}

void BTextEditorDocument::gotoNextBookmark()
{
    if ( _m_bookmarks.isEmpty() )
        return;
    _m_Bookmark bm = _m_bookmarks.takeFirst();
    if ( !bm.block.isValid() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.setPosition(bm.block.position() + bm.posInBlock);
    _m_edit->setTextCursor(tc);
    _m_bookmarks.append(bm);
}

void BTextEditorDocument::switchSelectedTextLayout(const BKeyboardLayoutMap &klm)
{
    if ( _m_edit->isReadOnly() || !klm.isValid() )
        return;
    QString text = selectedText();
    if ( !klm.switchLayout(text) )
        return;
    QTextCursor tc = _m_edit->textCursor();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    insertText(text);
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
}

//

void BTextEditorDocument::undo()
{
    if ( _m_edit->isReadOnly() )
        return;
    if ( isUndoAvailable() )
        _m_edit->undo();
    QTextCursor tc = _m_edit->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    if (tc.positionInBlock() <= i)
        return;
    tc.setPosition(tb.position() + i + 1);
    _m_edit->setTextCursor(tc);
    _m_editCursorPositionChanged();
}

void BTextEditorDocument::redo()
{
    if ( _m_edit->isReadOnly() )
        return;
    if ( isRedoAvailable() )
        _m_edit->redo();
    QTextCursor tc = _m_edit->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    if (tc.positionInBlock() <= i)
        return;
    tc.setPosition(tb.position() + i + 1);
    _m_edit->setTextCursor(tc);
    _m_editCursorPositionChanged();
}

void BTextEditorDocument::cut()
{
    if ( _m_edit->isReadOnly() )
        return;
    if ( _m_edit->extraSelections().isEmpty() )
        return;
    copy();
    _m_deleteSelection();
}

void BTextEditorDocument::copy()
{
    QString text = selectedText();
    if ( text.isEmpty() )
        return;
    QApplication::clipboard()->setText(text);
}

void BTextEditorDocument::paste()
{
    if ( _m_edit->isReadOnly() )
        return;
    QString text = QApplication::clipboard()->text();
    if ( text.isEmpty() )
        return;
    insertText(text);
}

void BTextEditorDocument::setFocusToEdit()
{
    _m_edit->setFocus();
}

//

void BTextEditorDocument::_m_initWidget()
{
    _m_edit = new BPlainTextEdit(this);
    //_m_edit->setCenterOnScroll(true);
    _m_highlighter = new BSyntaxHighlighter( _m_edit->document() );
    _m_highlighter->setSyntax(_m_syntax);
    _m_edit->installEventFilter(this);
    _m_edit->viewport()->installEventFilter(this);
    if ( parent() )
        _m_edit->viewport()->installEventFilter( parent() );
    _m_edit->setWordWrapMode(QTextOption::NoWrap);
    QPalette p = _m_edit->palette();
    _m_tcfHighlighted.setForeground( p.highlightedText() );
    _m_tcfHighlighted.setBackground( p.highlight() );
    p.setColor( QPalette::Highlight, QColor() );
    _m_edit->setPalette(p);
    _m_edit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( _m_edit, SIGNAL( copyAvailable(bool) ), this, SLOT( _m_cutAvailableChanged(bool) ) );
    connect( _m_edit, SIGNAL( copyAvailable(bool) ), this, SIGNAL( copyAvailableChanged(bool) ) );
    connect( _m_edit, SIGNAL( undoAvailable(bool) ), this, SLOT( _m_undoAvailableChanged(bool) ) );
    connect( _m_edit, SIGNAL( redoAvailable(bool) ), this, SLOT( _m_redoAvailableChanged(bool) ) );
    connect( _m_edit, SIGNAL( modificationChanged(bool) ), this, SLOT( _m_modificationChanged(bool) ) );
    connect( _m_edit, SIGNAL( cursorPositionChanged() ), this, SLOT( _m_editCursorPositionChanged() ) );
    connect( _m_edit, SIGNAL( selectionChanged() ), this, SLOT( _m_editSelectionChanged() ) );
    connect( _m_edit, SIGNAL( customContextMenuRequested(QPoint) ),
             this, SLOT( _m_editCustomContextMenuRequested(QPoint) ) );
    connect( _m_edit->document(), SIGNAL( blockCountChanged(int) ), this, SLOT( _m_documentBlockCountChanged(int) ) );
    _m_edit->setPlainText( QString().fill(' ', _m_lineLength) );
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::Start);
    _m_edit->setTextCursor(tc);
}

void BTextEditorDocument::_m_initMenu()
{
    _m_mnuContext = new QMenu(_m_edit);
    _m_initAction(_m_actUndo, "edit_undo", "Ctrl+Z");
    connect( _m_actUndo, SIGNAL( triggered() ), this, SLOT( undo() ) );
    connect( this, SIGNAL( undoAvailableChanged(bool) ), _m_actUndo, SLOT( setEnabled(bool) ) );
    _m_initAction(_m_actRedo, "edit_redo", "Ctrl+Shift+Z");
    connect( _m_actRedo, SIGNAL( triggered() ), this, SLOT( redo() ) );
    connect( this, SIGNAL( redoAvailableChanged(bool) ), _m_actRedo, SLOT( setEnabled(bool) ) );
    _m_mnuContext->addSeparator();
    _m_initAction(_m_actCut, "editcut", "Ctrl+X");
    connect( _m_actCut, SIGNAL( triggered() ), this, SLOT( cut() ) );
    connect( this, SIGNAL( cutAvailableChanged(bool) ), _m_actCut, SLOT( setEnabled(bool) ) );
    _m_initAction(_m_actCopy, "editcopy", "Ctrl+C");
    connect( _m_actCopy, SIGNAL( triggered() ), this, SLOT( copy() ) );
    connect( this, SIGNAL( copyAvailableChanged(bool) ), _m_actCopy, SLOT( setEnabled(bool) ) );
    _m_initAction( _m_actPaste, "editpaste", "Ctrl+V",
                   !_m_edit->isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
    connect( _m_actPaste, SIGNAL( triggered() ), this, SLOT( paste() ) );
    _m_initAction(_m_actDelete, "editdelete");
    connect( _m_actDelete, SIGNAL( triggered() ), this, SLOT( _m_deleteSelection() ) );
    _m_mnuContext->addSeparator();
    _m_initAction(_m_actSelectAll, "edit_select_all", "Ctrl+A", true);
    connect( _m_actSelectAll, SIGNAL( triggered() ), this, SLOT( _m_selectAll() ) );
}

void BTextEditorDocument::_m_initAction(QAction *&action, const QString &iconName,
                                        const QString &shortcut, bool enabled)
{
    action = new QAction(this);
    action->setEnabled(enabled);
    if ( !iconName.isEmpty() )
        action->setIcon( QIcon( BCore::beqtIcon(iconName) ) );
    if ( !shortcut.isEmpty() )
        action->setShortcut( QKeySequence(shortcut) );
    _m_mnuContext->addAction(action);
}

bool BTextEditorDocument::_m_setFileName(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    if (fileName == _m_fileName)
        return true;
    _m_fileName = fileName;
    emit fileNameChanged(_m_fileName);
    return true;
}

void BTextEditorDocument::_m_processSavedText(QString &text)
{
    removeSpaces(text);
}

void BTextEditorDocument::_m_addSpaces(QString &text) const
{
    QStringList sl = text.split('\n');
    for (int i = 0; i < sl.size(); ++i)
    {
        QString &line = sl[i];
        int llen = line.length();
        if (llen < _m_lineLength)
            line.append( QString().fill(' ', _m_lineLength - llen) );
    }
    text = sl.join("\n");
}

int BTextEditorDocument::_m_replaceInSelectionLines(const QString &text, const QString &newText,
                                                    Qt::CaseSensitivity cs)
{
    if (_m_blockMode)
        return -1;
    QTextCursor tc = _m_edit->textCursor();
    QString ntext = selectedText();
    if ( selectedText().isEmpty() )
        return -1;
    int count = ntext.count(text, cs);
    if (count < 1)
        return 0;
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    removeSpaces(ntext);
    ntext.replace(text, newText, cs);
    insertText(ntext);
    tc = _m_edit->textCursor();
    int pos = tc.position();
    tc.setPosition(end < start ? pos : start);
    tc.setPosition(end < start ? end : pos, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
    tc.endEditBlock();
    //Rehighlighting
    QTextBlock tb = _m_edit->document()->findBlock( qMin<int>(start, end) );
    QTextBlock tbe = _m_edit->document()->findBlock( qMax<int>(start, end) );
    while ( tb.isValid() && tb.blockNumber() < tbe.blockNumber() )
    {
        _m_highlighter->rehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

int BTextEditorDocument::_m_replaceInSelectionBlocks(const QString &text, const QString &newText,
                                                     Qt::CaseSensitivity cs)
{
    if (!_m_blockMode)
        return -1;
    _m_ExtraSelectionList esl = _m_edit->extraSelections();
    if ( esl.isEmpty() )
        return -1;
    int count = 0;
    int maxlen = 0;
    QStringList sl;
    for (int i = 0; i < esl.size(); ++i)
    {
        QTextCursor tce = esl.at(i).cursor;
        QString st = tce.selectedText();
        int slen = st.length();
        count += st.count(text, cs);
        removeSpaces(st);
        st.replace(text, newText, cs);
        if (st.length() > maxlen)
            maxlen = st.length();
        QTextBlock tbe = tce.block();
        QString txt = tbe.text();
        removeSpaces(txt);
        sl << txt.replace(tce.selectionStart() - tbe.position(), slen, st);
        if (txt.length() > _m_lineLength)
        {
            emit maxLineLengthReached();
            return -1;
        }
    }
    QTextCursor tc = _m_edit->textCursor();
    tc.beginEditBlock();
    int offset = esl.first().cursor.selectionStart() - esl.first().cursor.block().position();
    int bpos = tc.block().position();
    int dlen = maxlen - esl.first().cursor.selectedText().length();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    for (int i = 0; i < esl.size(); ++i)
    {
        QTextCursor tce = esl.at(i).cursor;
        QString txt = sl.at(i);
        _m_addSpaces(txt);
        tce.movePosition(QTextCursor::StartOfBlock);
        tce.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tce.insertText(txt);
    }
    if (start < end)
    {
        if (start > bpos + offset)
            start += dlen;
        else
            end += dlen;
    }
    else
    {
        if (end > bpos + offset)
            end += dlen;
        else
            start += dlen;
    }
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
    tc.endEditBlock();
    //Rehighlighting
    for (int i = 0; i < esl.size(); ++i)
        _m_highlighter->rehighlightBlock( esl.at(i).cursor.block() );
    return count;
}

bool BTextEditorDocument::_m_handleKeyPressEvent(QObject *object, QKeyEvent *event)
{
    if (_m_recorder)
        _m_recorder->handleKeyPress(event);
    int modifiers = event->modifiers();
    int key = event->key();
    static const int ContorlShiftModifier = ( (int) Qt::ControlModifier | (int) Qt::ShiftModifier );
    static const int KeypadControlModifier = ( (int) Qt::KeypadModifier | (int) Qt::ControlModifier);
    switch (modifiers)
    {
    case Qt::NoModifier:
        switch (key)
        {
        case Qt::Key_Return:
            _m_handleReturn();
            return true;
        case Qt::Key_Backspace:
            _m_handleBackspace();
            return true;
        case Qt::Key_Delete:
            _m_handleDelete();
            return true;
        case Qt::Key_End:
            _m_handleEnd();
            return true;
        case Qt::Key_Left:
            _m_handleLeft();
            return true;
        default:
            break;
        }
        break;
    case Qt::ControlModifier:
        switch (key)
        {
        case Qt::Key_Z:
            undo();
            return true;
        case Qt::Key_X:
            cut();
            return true;
        case Qt::Key_C:
            copy();
            return true;
        case Qt::Key_V:
            paste();
            return true;
        case Qt::Key_A:
            _m_selectAll();
            return true;
        case Qt::Key_End:
            _m_handleEnd(true);
            return true;
        case Qt::Key_Tab:
            emit requestSwitchDocument();
            return true;
        case Qt::Key_Backspace:
            _m_handleCtrlBackspace();
            return true;
        case Qt::Key_Delete:
            _m_handleCtrlDelete();
            return true;
        case Qt::Key_Left:
            _m_handleCtrlLeft();
            return true;
        case Qt::Key_Right:
            _m_handleCtrlRight();
            return true;
        default:
            break;
        }
        break;
    case Qt::ShiftModifier:
        switch (key)
        {
        case Qt::Key_Left:
            _m_handleLeft(true);
            return true;
        case Qt::Key_End:
            _m_handleShiftEnd();
            return true;
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            QTimer::singleShot( 0, this, SLOT( _m_editSelectionChanged() ) );
            return false;
        default:
            break;
        }
        break;
    case Qt::AltModifier:
        switch (key)
        {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            _m_move(key);
            return true;
        default:
            break;
        }
        break;
    case ContorlShiftModifier:
        switch (key)
        {
        case Qt::Key_Z:
            redo();
            return true;
        default:
            break;
        }
        break;
    case Qt::KeypadModifier:
        switch (key)
        {
        case Qt::Key_Enter:
            _m_handleReturn();
            return true;
        case Qt::Key_End:
            _m_handleEnd();
            return true;
        default:
            break;
        }
        break;
    case KeypadControlModifier:
        switch (key)
        {
        case Qt::Key_End:
            _m_handleEnd(true);
            return true;
        default:
            break;
        }
        break;
    default:
        break;
    }
    QString text = event->text();
    if ( !text.isEmpty() )
    {
        _m_handleSymbol( text.at(0) );
        return true;
    }
    return QObject::eventFilter(object, event);
}

bool BTextEditorDocument::_m_handleMousePressEvent(QObject *object, QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QTextCursor tc = _m_edit->cursorForPosition( event->pos() );
        if (tc.positionInBlock() == _m_lineLength)
        {
            QTextBlock tb = tc.block();
            if ( !tb.next().isValid() )
            {
                QString text = tb.text();
                removeSpaces(text);
                tc.setPosition( tb.position() + text.length() );
                _m_edit->setTextCursor(tc);
                return true;
            }
        }
    }
    return QObject::eventFilter(object, event);
}

bool BTextEditorDocument::_m_handleMouseDoubleClickEvent(QObject *object, QMouseEvent *event)
{
    QTextCursor tc = _m_edit->textCursor();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int lposb = posb - 1;
    if ( (lposb < 0 || text.at(lposb) == ' ') && text.at(posb) == ' ' )
        return true;
    return QObject::eventFilter(object, event);
}

void BTextEditorDocument::_m_handleReturn()
{
    if ( _m_edit->isReadOnly() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.beginEditBlock();
    _m_deleteSelection();
    tc = _m_edit->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tbp = tc.block();
    QString text = tbp.text();
    int i = 0;
    while (i < posb && text.at(i) == ' ')
        ++i;
    QString ltext = text.left(posb);
    QString rtext = text.right(text.length() - posb);
    rtext.prepend( QString().fill(' ', i) );
    _m_addSpaces(ltext);
    _m_addSpaces(rtext);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(ltext);
    tc.insertBlock();
    tc.insertText(rtext);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.setPosition(tc.position() + i);
    _m_edit->setTextCursor(tc);
    tc.endEditBlock();
    _m_highlighter->rehighlightBlock(tbp); //
}

void BTextEditorDocument::_m_handleEnd(bool ctrl)
{
    QTextCursor tc = _m_edit->textCursor();
    QTextBlock tb = ctrl ? _m_edit->document()->lastBlock() : tc.block();
    QString text = tb.text();
    int i = text.length() - 1;
    while (i >= 0 && text.at(i) == ' ')
        --i;
    tc.setPosition(tb.position() + i + 1);
    _m_edit->setTextCursor(tc);
    _m_editCursorPositionChanged();
}

void BTextEditorDocument::_m_handleShiftEnd()
{
    QTextCursor tc = _m_edit->textCursor();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    removeSpaces(text);
    int d = text.length() - tc.positionInBlock();
    if (d < 1)
        return;
    tc.setPosition(tc.position() + d, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    _m_editCursorPositionChanged();
}

void BTextEditorDocument::_m_handleBackspace()
{
    if ( _m_edit->isReadOnly() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    if ( tc.hasSelection() )
        return _m_deleteSelection();
    if (tc.position() == 0)
        return;
    int posb = tc.positionInBlock();
    if (posb)
    {
        tc.beginEditBlock();
        QString text = tc.block().text();
        int pos = tc.position() - 1;
        text.remove(posb - 1, 1);
        text.append(' ');
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
        _m_edit->setTextCursor(tc);
        tc.endEditBlock();
    }
    else
    {
        QTextBlock tb = tc.block();
        QString text = tb.text();
        QString ptext = tb.previous().text();
        removeSpaces(text);
        removeSpaces(ptext);
        int len = ptext.length();
        text.prepend(ptext);
        if (text.length() > _m_lineLength)
        {
            emit maxLineLengthReached();
            return;
        }
        tc.beginEditBlock();
        _m_addSpaces(text);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.removeSelectedText();
        tc.deletePreviousChar();
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(tc.block().position() + len);
        _m_edit->setTextCursor(tc);
        tc.endEditBlock();
    }
}

void BTextEditorDocument::_m_handleDelete()
{
    if ( _m_edit->isReadOnly() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    if ( tc.hasSelection() )
        return _m_deleteSelection();
    if (tc.position() == 0)
        return;
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int pos = tc.position();
    QString t = text;
    removeSpaces(t);
    if ( posb < t.length() )
    {
        tc.beginEditBlock();
        text.remove(posb, 1);
        text.append(' ');
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
        _m_edit->setTextCursor(tc);
        tc.endEditBlock();
    }
    else if ( tb.next().isValid() )
    {
        text.remove(posb, text.length() - posb);
        text.append( tb.next().text() );
        removeSpaces(text);
        if (text.length() > _m_lineLength)
        {
            emit maxLineLengthReached();
            return;
        }
        tc.beginEditBlock();
        _m_addSpaces(text);
        tc.movePosition(QTextCursor::StartOfBlock);
        tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        tc.insertText(text);
        tc.setPosition(pos);
        _m_edit->setTextCursor(tc);
        tc.endEditBlock();
    }
}

void BTextEditorDocument::_m_handleCtrlBackspace()
{
    if ( _m_edit->isReadOnly() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    if ( tc.hasSelection() )
        return _m_deleteSelection();
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (--i >= 0 && text.at(i) == ' ')
        ;
    while (--i >= 0 && text.at(i) != ' ')
        ;
    i += 1;
    text.remove(i, posb - i);
    _m_addSpaces(text);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(text);
    tc.setPosition(tc.block().position() + i);
    _m_edit->setTextCursor(tc);
    tc.endEditBlock();
}

void BTextEditorDocument::_m_handleCtrlDelete()
{
    if ( _m_edit->isReadOnly() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    if ( tc.hasSelection() )
        return _m_deleteSelection();
    tc.beginEditBlock();
    QString text = tc.block().text();
    int posb = tc.positionInBlock();
    int i = posb;
    while (++i < text.length() && text.at(i) == ' ')
        ;
    while (++i < text.length() && text.at(i) != ' ')
        ;
    text.remove(posb, i - posb);
    _m_addSpaces(text);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(text);
    tc.setPosition(tc.block().position() + posb);
    _m_edit->setTextCursor(tc);
    tc.endEditBlock();
}

void BTextEditorDocument::_m_handleCtrlLeft()
{
    QTextCursor tc = _m_edit->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int bpos = tb.position();
    removeSpaces(text);
    if (!posb)
    {
        QTextBlock tbp = tb.previous();
        if ( tbp.isValid() )
        {
            QString textp = tbp.text();
            removeSpaces(textp);
            tc.setPosition( tbp.position() + textp.length() );
            _m_edit->setTextCursor(tc);
        }
        return;
    }
    if (text.length() < posb)
    {
        tc.setPosition( bpos + text.length() );
        _m_edit->setTextCursor(tc);
        return;
    }
    if ( text.at(posb - 1).isLetterOrNumber() )
        while ( posb > 0 && text.at(posb - 1).isLetterOrNumber() )
            --posb;
    else if ( text.at(posb - 1).isSpace() )
        while ( posb > 0 && text.at(posb - 1).isSpace() )
            --posb;
    else if ( text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct() )
        while ( posb > 0 && ( text.at(posb - 1).isSymbol() || text.at(posb - 1).isPunct() ) )
            --posb;
    while ( posb > 0 && text.at(posb - 1).isSpace() )
        --posb;
    tc.setPosition(bpos + posb);
    _m_edit->setTextCursor(tc);
}

void BTextEditorDocument::_m_handleCtrlRight()
{
    QTextCursor tc = _m_edit->textCursor();
    int posb = tc.positionInBlock();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int bpos = tb.position();
    removeSpaces(text);
    if (text.length() <= posb)
    {
        QTextBlock tbn = tb.next();
        if ( tbn.isValid() )
            tc.setPosition( tbn.position() );
        else
            tc.setPosition( bpos + text.length() );
        _m_edit->setTextCursor(tc);
        return;
    }
    if ( text.at(posb).isLetterOrNumber() )
        while ( posb < text.length() && text.at(posb).isLetterOrNumber() )
            ++posb;
    else if ( text.at(posb).isSpace() )
        while ( posb < text.length() && text.at(posb).isSpace() )
            ++posb;
    else if ( text.at(posb).isSymbol() || text.at(posb).isPunct() )
        while ( posb < text.length() && ( text.at(posb).isSymbol() || text.at(posb).isPunct() ) )
            ++posb;
    while ( posb < text.length() && text.at(posb).isSpace() )
        ++posb;
    tc.setPosition(bpos + posb);
    _m_edit->setTextCursor(tc);
}

void BTextEditorDocument::_m_handleLeft(bool shift)
{
    QTextCursor tc = _m_edit->textCursor();
    int pos = tc.position();
    if (!pos)
    {
        if (!shift)
        {
            tc.setPosition(0);
            _m_edit->setTextCursor(tc);
            _m_editSelectionChanged();
        }
        return;
    }
    QTextCursor::MoveMode mode = shift ?
                QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;
    int npos;
    if ( tc.positionInBlock() )
    {
        npos = pos - 1;
    }
    else
    {
        QTextBlock tb = tc.block().previous();
        //check block validity
        QString text = tb.text();
        int i = text.length() - 1;
        while (i >= 0 && text.at(i) == ' ')
            --i;
        npos = tb.position() + i + 1;
    }
    tc.setPosition(npos, mode);
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
}

void BTextEditorDocument::_m_handleSymbol(const QChar &c)
{
    if ( _m_edit->isReadOnly() || (!c.isPrint() && c != '\t') )
        return;
    QTextCursor tc = _m_edit->textCursor();
    bool b = isCopyAvailable();
    if (b)
    {
        tc.beginEditBlock();
        _m_deleteSelection();
        tc = _m_edit->textCursor();
    }
    int posb = tc.positionInBlock();
    if (posb >= _m_lineLength)
    {
        emit maxLineLengthReached();
        if (b)
            tc.endEditBlock();
        return;
    }
    int pos = tc.position();
    QTextBlock tb = tc.block();
    QString text = tb.text();
    int spcount = _m_tabWidth - (posb < _m_tabWidth ? posb : posb % _m_tabWidth);
    QString ntext = (c == '\t') ? QString().fill(' ', spcount ? spcount : _m_tabWidth) : QString(c);
    int nlen = ntext.length();
    if ( text.right(nlen) != QString().fill(' ', nlen) )
    {
        emit maxLineLengthReached();
        if (b)
            tc.endEditBlock();
        return;
    }
    if (!b)
        tc.beginEditBlock();
    text.remove(text.length() - nlen, nlen);
    text.insert(posb, ntext);
    tc.movePosition(QTextCursor::StartOfBlock);
    tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    tc.insertText(text);
    tc.setPosition(pos + nlen);
    _m_edit->setTextCursor(tc);
    tc.endEditBlock();
}

void BTextEditorDocument::_m_move(int key)
{
    if ( _m_edit->isReadOnly() )
        return;
    _m_ExtraSelectionList esl = _m_edit->extraSelections();
    if ( esl.isEmpty() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    if (_m_blockMode)
    {
        if (Qt::Key_Left == key || Qt::Key_Right == key)
        {
            for (int i = 0; i < esl.size(); ++i)
            {
                QTextCursor tce = esl.at(i).cursor;
                QString text = tce.selectedText();
                if (Qt::Key_Left == key)
                    text.remove(0, 1).append(' ');
                else
                    text.remove(text.length() - 1, 1).prepend(' ');
                tce.insertText(text);
            }
        }
        else if (Qt::Key_Up == key || Qt::Key_Down == key)
        {
            const int lb = (Qt::Key_Up == key) ? 0 : esl.size() - 1;
            const int ub = (Qt::Key_Up == key) ? esl.size() - 1 : 0;
            const int m = (Qt::Key_Up == key) ? 1 : -1;
            for (int i = lb; i != ub; i += m)
            {
                QTextCursor tce1 = esl.at(i).cursor;
                QTextCursor tce2 = esl.at(i + m).cursor;
                int starte = tce1.selectionStart();
                int ende = tce1.selectionEnd();
                tce1.insertText( tce2.selectedText() );
                tce1.setPosition(starte);
                tce1.setPosition(ende, QTextCursor::KeepAnchor);
                esl[i].cursor = tce1;
            }
            QTextCursor tce = esl.at(ub).cursor;
            tce.insertText( QString().fill( ' ', tce.selectionEnd() - tce.selectionStart() ) );
        }
    }
    else
    {
        QTextCursor tce = esl.first().cursor;
        QString text = tce.selectedText();
        if (Qt::Key_Left == key || Qt::Key_Right == key)
        {
            QStringList sl = text.split(QChar::ParagraphSeparator);
            if (Qt::Key_Left == key)
            {
                for (int i = 0; i < sl.size(); ++i)
                {
                    QString &s = sl[i];
                    if ( !s.isEmpty() )
                        s.remove(0, 1).append(' ');
                }
            }
            else
            {
               for (int i = 0; i < sl.size(); ++i)
               {
                   QString &s = sl[i];
                   if ( !s.isEmpty() )
                       s.remove(sl.at(i).length() - 1, 1).prepend(' ');
               }
            }
            tce.insertText( sl.join("\n") );
        }
        else if (Qt::Key_Up == key)
        {
            tce.insertText( text.toUpper() );
        }
        else if (Qt::Key_Down == key)
        {
            tce.insertText( text.toLower() );
        }
    }
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
    tc.endEditBlock();
}

void BTextEditorDocument::_m_highlightBrackets()
{
    for (int i = 0; i < _m_highlightPosList.size(); ++i)
        _m_highlightBracket(_m_highlightPosList.at(i), false, false);
    _m_highlightPosList.clear();
    QTextCursor tc = _m_edit->textCursor();
    _m_highlighter->rehighlightBlock( tc.block() );
    BSyntax::CursorProcessingResult r = _m_syntax.processCursor(tc);
    _m_highlightPosList << r.positions.keys();
    for (int i = 0; i < _m_highlightPosList.size(); ++i)
    {
        int key = _m_highlightPosList.at(i);
        _m_highlightBracket( key, true, r.positions.value(key) );
    }
}


void BTextEditorDocument::_m_highlightBracket(int pos, bool highlight, bool error)
{
    QTextBlock tb = _m_edit->document()->findBlock(pos);
    if ( !tb.isValid() )
        return;
    QTextLayout *tl = tb.layout();
    QList<QTextLayout::FormatRange> frl = tl->additionalFormats();
    QTextLayout::FormatRange fr;
    fr.format.setBackground(error ? BSyntax::BracketsErrorColor : BSyntax::BracketsColor);
    fr.length = 1;
    fr.start = pos - tb.position();
    for (int i = 0; i < frl.size(); ++i)
    {
        const QTextLayout::FormatRange frc = frl.at(i);
        if (frc.format.background() != QTextCharFormat().background() &&
                frc.length == fr.length && frc.start == fr.start)
        {
            frl.removeAt(i);
            break;
        }
    }
    if (highlight)
        frl << fr;
    tl->setAdditionalFormats(frl);
    _m_edit->document()->markContentsDirty(pos, 1);
}

//

void BTextEditorDocument::_m_retranslateUi()
{
    _m_actUndo->setText( tr("Undo action", "act text") );
    _m_actRedo->setText( tr("Redo action", "act text") );
    _m_actCut->setText( tr("Cut", "act text") );
    _m_actCopy->setText( tr("Copy", "act text") );
    _m_actPaste->setText( tr("Paste", "act text") );
    _m_actDelete->setText( tr("Delete", "act text") );
    _m_actSelectAll->setText( tr("Select all", "act text") );
}

void BTextEditorDocument::_m_cutAvailableChanged(bool available)
{
    emit cutAvailableChanged(!_m_edit->isReadOnly() && available);
}

void BTextEditorDocument::_m_undoAvailableChanged(bool available)
{
    emit undoAvailableChanged(!_m_edit->isReadOnly() && available);
}

void BTextEditorDocument::_m_redoAvailableChanged(bool available)
{
    emit redoAvailableChanged(!_m_edit->isReadOnly() && available);
}

void BTextEditorDocument::_m_modificationChanged(bool modified)
{
    emit modificationChanged(!_m_edit->isReadOnly() && modified);
}

void BTextEditorDocument::_m_documentBlockCountChanged(int count)
{
    //
}

void BTextEditorDocument::_m_editCursorPositionChanged()
{
    _m_highlightBrackets();
    QTextCursor tc = _m_edit->textCursor();
    emit cursorPositionChanged( tc.blockNumber(), tc.positionInBlock() );
}

void BTextEditorDocument::_m_editSelectionChanged()
{
    QTextCursor tc = _m_edit->textCursor();
    bool b = tc.hasSelection();
    _m_actDelete->setEnabled(!_m_edit->isReadOnly() && b);
    emit selectionChanged(b);
    if (!b)
    {
        _m_edit->setExtraSelections( _m_ExtraSelectionList() );
        return;
    }
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    _m_ExtraSelectionList esl;
    _m_ExtraSelection es;
    es.format = _m_tcfHighlighted;
    if (_m_blockMode)
    {
        int soffset = start - _m_edit->document()->findBlock(start).position();
        int eoffset = end - _m_edit->document()->findBlock(end).position();
        if (soffset == eoffset)
        {
            _m_edit->setExtraSelections( _m_ExtraSelectionList() );
            return;
        }
        int minoffset = qMin<int>(soffset, eoffset);
        int maxoffset = qMax<int>(soffset, eoffset);
        int astart = qMin<int>(start, end);
        int aend = qMax<int>(start, end);
        QTextBlock tb = _m_edit->document()->findBlock(astart);
        while ( tb.isValid() )
        {
            int bpos = tb.position();
            if (bpos > aend)
                break;
            tc.setPosition(bpos + minoffset);
            tc.setPosition(bpos + maxoffset, QTextCursor::KeepAnchor);
            es.cursor = tc;
            esl << es;
            tb = tb.next();
        }
    }
    else
    {
        tc.setPosition( qMin<int>(start, end) );
        tc.setPosition(qMax<int>(start, end), QTextCursor::KeepAnchor);
        es.cursor = tc;
        esl << es;
    }
    _m_edit->setExtraSelections(esl);
}

void BTextEditorDocument::_m_editCustomContextMenuRequested(const QPoint &pos)
{
    _m_mnuContext->popup( _m_edit->mapToGlobal(pos) );
}

void BTextEditorDocument::_m_deleteSelection()
{
    _m_ExtraSelectionList esl = _m_edit->extraSelections();
    if ( esl.isEmpty() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.beginEditBlock();
    _m_ExtraSelection esf = esl.first();
    int pos = esf.cursor.selectionStart();
    if (_m_blockMode)
    {
        for (int i = 0; i < esl.size(); ++i)
        {
            _m_ExtraSelection &es = esl[i];
            QTextCursor tce = es.cursor;
            QTextBlock tbe = tce.block();
            QString text = tbe.text();
            int bpos = tbe.position();
            int start = tce.selectionStart() - bpos;
            int end = tce.selectionEnd() - bpos;
            text.remove(start, end - start);
            _m_addSpaces(text);
            es.cursor.movePosition(QTextCursor::StartOfBlock);
            es.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            es.cursor.insertText(text);
        }
    }
    else
    {
        esf.cursor.removeSelectedText();
        QString text = esf.cursor.block().text();
        if (text.length() > _m_lineLength)
            text.truncate(_m_lineLength);
        _m_addSpaces(text);
        esf.cursor.movePosition(QTextCursor::StartOfBlock);
        esf.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        esf.cursor.insertText(text);
    }
    _m_edit->setExtraSelections( _m_ExtraSelectionList() );
    tc.setPosition(pos);
    _m_edit->setTextCursor(tc);
    tc.endEditBlock();
}

void BTextEditorDocument::_m_selectAll()
{
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::Start);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
}

void BTextEditorDocument::_m_futureWatcherFinished()
{
    _m_FutureWatcher *futw = dynamic_cast<_m_FutureWatcher *>( sender() );
    if (!futw)
        return;
    _m_ProcessLoadedTextResult result = futw->result();
    futw->deleteLater();
    _m_edit->setEnabled(true);
    _m_highlighter->setDocument(0);
    _m_edit->setPlainText(result.text);
    _m_highlighter->setDocument( _m_edit->document() );
    _m_edit->document()->setModified(false);
    _m_edit->setFocus();
    if ( !result.lineIndexes.isEmpty() )
    {
        BSplittedLinesDialog *sld = new BSplittedLinesDialog(result.lineIndexes, _m_edit);
        sld->setAttribute(Qt::WA_DeleteOnClose, true);
        connect( sld, SIGNAL( gotoLine(int, bool) ), this, SLOT( _m_gotoLine(int, bool) ) );
        sld->show();
    }
}

void BTextEditorDocument::_m_gotoLine(int index, bool select)
{
    QTextBlock tb = _m_edit->document()->findBlockByLineNumber(index);
    if ( !tb.isValid() )
        return;
    QTextCursor tc = _m_edit->textCursor();
    tc.setPosition( tb.position() );
    if (select)
    {
        QString text = tb.text();
        removeSpaces(text);
        tc.setPosition(tc.position() + text.length(), QTextCursor::KeepAnchor);
    }
    _m_edit->setTextCursor(tc);
    _m_editSelectionChanged();
    _m_edit->window()->raise();
    _m_edit->window()->activateWindow();
    _m_edit->setFocus();
}
