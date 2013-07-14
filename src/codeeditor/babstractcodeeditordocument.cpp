#include "babstractcodeeditordocument.h"
#include "babstractcodeeditordocument_p.h"
#include "babstractdocumentdriver.h"
#include "babstractfiletype.h"
#include "bcodeeditor.h"
#include "btextblockuserdata.h"
#include "bcodeedit.h"
#include "bspellchecker.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQt>
#include <BeQtWidgets/BPlainTextEdit>

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QVBoxLayout>
#include <QPoint>
#include <QFont>
#include <QTextDocument>
#include <QString>
#include <QList>
#include <QTextCodec>
#include <QDir>
#include <QMetaObject>
#include <QByteArray>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QTextEdit>
#include <QTextFormat>
#include <QTextCursor>
#include <QBrush>
#include <QPlainTextEdit>
#include <QTextBlockUserData>
#include <QDebug>
#include <QRegExp>
#include <QMenu>
#include <QAction>
#include <QPair>
#include <QVariant>

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

void BSyntaxHighlighter::setCurrentBlockState(int newState)
{
    QSyntaxHighlighter::setCurrentBlockState(newState);
}

void BSyntaxHighlighter::setCurrentBlockUserData(BTextBlockUserData *data)
{
    QSyntaxHighlighter::setCurrentBlockUserData(data);
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

QTextBlock BSyntaxHighlighter::currentBlock() const
{
    return QSyntaxHighlighter::currentBlock();
}

int BSyntaxHighlighter::currentBlockState() const
{
    return QSyntaxHighlighter::currentBlockState();
}

BTextBlockUserData *BSyntaxHighlighter::currentBlockUserData() const
{
    return dynamic_cast<BTextBlockUserData *>(QSyntaxHighlighter::currentBlockUserData());
}

QTextCharFormat BSyntaxHighlighter::format(int position) const
{
    return QSyntaxHighlighter::format(position);
}

int BSyntaxHighlighter::previousBlockState() const
{
    return QSyntaxHighlighter::previousBlockState();
}

BAbstractCodeEditorDocument *BSyntaxHighlighter::editorDocument() const
{
    return EditorDocument;
}

/*============================== Protected methods =========================*/

void BSyntaxHighlighter::highlightBlock(const QString &text)
{
    BAbstractFileType *ft = EditorDocument ? EditorDocument->fileType() : 0;
    if (ft)
    {
        ft->setCurrentHighlighter(this);
        ft->highlightBlock(text);
        ft->setCurrentHighlighter(0);
    }
    BSpellChecker *sc = EditorDocument ? EditorDocument->spellChecker() : 0;
    if (sc)
    {
        int i = 0;
        QString w;
        while (i <= text.length())
        {
            if (i < text.length() && (text.at(i).isLetterOrNumber() || text.at(i) == '_'))
            {
                w += text.at(i);
            }
            else if (!w.isEmpty())
            {
                if (!sc->spell(w))
                {
                    foreach (int j, bRangeD(i - w.length(), i - 1))
                    {
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
}

/*============================================================================
================================ BAbstractCodeEditorDocumentPrivate ==========
============================================================================*/

/*============================== Static public methods =====================*/

void BAbstractCodeEditorDocumentPrivate::removeExtraSelections(ExtraSelectionList &from,
                                                               const ExtraSelectionList &what)
{
    foreach (const QTextEdit::ExtraSelection &es, what)
    {
        for (int i = from.size() - 1; i >= 0; --i)
        {
            if (from.at(i).cursor == es.cursor && from.at(i).format == es.format)
                from.removeAt(i);
        }
    }
}

QTextCharFormat BAbstractCodeEditorDocumentPrivate::createBracketsFormat()
{
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor("red")));
    return fmt;
}

QTextCharFormat BAbstractCodeEditorDocumentPrivate::createBracketsErrorFormat()
{
    QTextCharFormat fmt;
    fmt.setBackground(QBrush(QColor("hotpink")));
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

/*============================== Public constructors =======================*/

BAbstractCodeEditorDocumentPrivate::BAbstractCodeEditorDocumentPrivate(BAbstractCodeEditorDocument *q,
                                                                       BCodeEditor *editor) :
    BBasePrivate(q), Editor(editor)
{
    //
}

BAbstractCodeEditorDocumentPrivate::~BAbstractCodeEditorDocumentPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractCodeEditorDocumentPrivate::init()
{
    QVBoxLayout *vlt = new QVBoxLayout(q_func());
      vlt->setContentsMargins(0, 0, 0, 0);
    edit = 0;
    highlighter = 0;
    fileType = BAbstractFileType::defaultFileType();
    recognizedBrackets = fileType->brackets();
    bracketsHighlighting = true;
    spellChecker = 0;
    codec = QTextCodec::codecForName("UTF-8");
    asyncMin = 100 * BeQt::Kilobyte;
    readOnly = false;
    isModified = false;
    hasSelection = false;
    cutAvailable = false;
    copyAvailable = false;
    pasteAvailable = false;
    undoAvailable = false;
    redoAvailable = false;
    buisy = false;
}

bool BAbstractCodeEditorDocumentPrivate::createEdit()
{
    QTextDocument *doc = 0;
    edit = q_func()->createEdit(&doc);
    if (!edit)
        return false;
    static_cast<QVBoxLayout *>(q_func()->layout())->addWidget(edit);
    if (doc)
        highlighter = new BSyntaxHighlighter(q_func(), doc);
    return true;
}

void BAbstractCodeEditorDocumentPrivate::setBuisy(bool b)
{
    if (b == buisy)
        return;
    buisy = b;
    QMetaObject::invokeMethod(q_func(), "buisyChanged", Q_ARG(bool, b));
}

void BAbstractCodeEditorDocumentPrivate::setFileName(QString fn)
{
    fn = QDir::fromNativeSeparators(fn);
    bool b = (fn != fileName);
    fileName = fn;
    if (b)
        QMetaObject::invokeMethod(q_func(), "fileNameChanged", Q_ARG(QString, fn));
}

void BAbstractCodeEditorDocumentPrivate::setCodec(QTextCodec *c)
{
    if (!c)
        return;
    bool b = (c != codec);
    codec = c;
    if (b)
        QMetaObject::invokeMethod(q_func(), "codecChanged", Q_ARG(QString, BCodeEditor::codecName(c)));
}

void BAbstractCodeEditorDocumentPrivate::rehighlight()
{
    if (!highlighter)
        return;
    highlighter->rehighlight();
    q_func()->highlightBrackets();
}

void BAbstractCodeEditorDocumentPrivate::highlightBrackets()
{
    bool ok = false;
    ExtraSelectionList selections = getExtraSelections(&ok);
    if (!ok)
        return;
    removeExtraSelections(selections, highlightedBrackets);
    highlightedBrackets.clear();
    if (!bracketsHighlighting)
        return setExtraSelections(selections);
    FindBracketPairResultList list;
    list << findLeftBracketPair();
    list << findRightBracketPair();
    foreach (const FindBracketPairResult &res, list)
    {
        if (res.start >= 0 && res.end >= 0)
        {
            if (BAbstractFileType::areEqual(*res.startBr, *res.endBr))
            {
                QTextEdit::ExtraSelection ess = createExtraSelection(createBracketsFormat());
                ess.cursor.setPosition(res.start);
                ess.cursor.setPosition(res.start + res.startBr->opening.length(), QTextCursor::KeepAnchor);
                highlightedBrackets << ess;
                selections << ess;
                QTextEdit::ExtraSelection ese = createExtraSelection(createBracketsFormat());
                ese.cursor.setPosition( res.end - res.endBr->closing.length() );
                ese.cursor.setPosition(res.end, QTextCursor::KeepAnchor);
                highlightedBrackets << ese;
                selections << ese;
            }
            else
            {
                QTextEdit::ExtraSelection es = createExtraSelection(createBracketsErrorFormat());
                es.cursor.setPosition(res.start);
                es.cursor.setPosition(res.end, QTextCursor::KeepAnchor);
                highlightedBrackets << es;
                selections << es;
            }
        }
    }
    setExtraSelections(selections);
}

void BAbstractCodeEditorDocumentPrivate::setExtraSelections(const ExtraSelectionList &list, bool *ok)
{
    if (ok)
        *ok = true;
    QTextEdit *tedt = qobject_cast<QTextEdit *>(q_func()->innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(q_func()->innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(q_func()->innerEdit());
    if (tedt)
        tedt->setExtraSelections(list);
    else if (ptedt)
        ptedt->setExtraSelections(list);
    else if (cedt)
        cedt->setExtraSelections(list);
    else if (ok)
        *ok = false;
}

BAbstractCodeEditorDocumentPrivate::ExtraSelectionList
    BAbstractCodeEditorDocumentPrivate::getExtraSelections(bool *ok) const
{
    if (ok)
        *ok = true;
    QTextEdit *tedt = qobject_cast<QTextEdit *>(q_func()->innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(q_func()->innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(q_func()->innerEdit());
    if (tedt)
        return tedt->extraSelections();
    else if (ptedt)
        return ptedt->extraSelections();
    else if (cedt)
        return cedt->extraSelections();
    else
        return bRet(ok, false, ExtraSelectionList());
}

BAbstractCodeEditorDocumentPrivate::ExtraSelection
    BAbstractCodeEditorDocumentPrivate::createExtraSelection(const QTextCharFormat &format, bool *ok) const
{
    ExtraSelection r;
    r.cursor = q_func()->textCursor(ok);
    r.format = format;
    return bRet(ok, !r.cursor.isNull(), r);
}

BAbstractCodeEditorDocumentPrivate::ExtraSelection
    BAbstractCodeEditorDocumentPrivate::createExtraSelection(bool *ok) const
{
    return createExtraSelection(ok);
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
    if (!testBracket(BTextBlockUserData::textWithoutComments(tb), posInBlock, false, bracket))
        return res;
    res.end = tb.position() + posInBlock;
    posInBlock -= bracket->closing.length();
    int depth = 1;
    const BracketPair *br = 0;
    while (tb.isValid())
    {
        QString text = BeQt::removeTrailingSpaces(BTextBlockUserData::textWithoutComments(tb));
        while (posInBlock >= 0)
        {
            if (testBracket(text, posInBlock, true, br))
            {
                --depth;
                if (!depth)
                {
                    res.start = tb.position() + posInBlock;
                    res.startBr = br;
                    res.endBr = bracket;
                    return res;
                }
                if (testBracket(text, posInBlock, false, br))
                {
                    ++depth;
                    posInBlock -= br->closing.length();
                }
                else
                {
                    --posInBlock;
                }
            }
            else if (testBracket(text, posInBlock, false, br))
            {
                ++depth;
                posInBlock -= br->closing.length();
            }
            else
            {
                --posInBlock;
            }
        }
        tb = tb.previous();
        int skipFrom = BTextBlockUserData::blockSkipFrom(tb);
        posInBlock = (skipFrom >= 0) ? (skipFrom - 1) : tb.length();
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
    if (!testBracket(BTextBlockUserData::textWithoutComments(tb), posInBlock, true, bracket))
        return res;
    res.start = tb.position() + posInBlock;
    posInBlock += bracket->opening.length();
    int depth = 1;
    const BracketPair *br = 0;
    while ( tb.isValid() )
    {
        QString text = BeQt::removeTrailingSpaces(BTextBlockUserData::textWithoutComments(tb));
        while (posInBlock <= text.length())
        {
            if (testBracket(text, posInBlock, false, br))
            {
                --depth;
                if (!depth)
                {
                    res.end = tb.position() + posInBlock;
                    res.startBr = bracket;
                    res.endBr = br;
                    return res;
                }
                if (testBracket(text, posInBlock, true, br))
                {
                    ++depth;
                    posInBlock += br->opening.length();
                }
                else
                {
                    ++posInBlock;
                }
            }
            else if (testBracket(text, posInBlock, true, br))
            {
                ++depth;
                posInBlock += br->opening.length();
            }
            else
            {
                ++posInBlock;
            }
        }
        tb = tb.next();
        posInBlock = 0;
    }
    return res;
}

bool BAbstractCodeEditorDocumentPrivate::testBracket(const QString &text, int posInBlock, bool opening,
                                                     const BracketPair *&bracket) const
{
    bracket = 0;
    if (recognizedBrackets.isEmpty())
        return false;
    foreach (const BracketPair &br, recognizedBrackets)
    {
        const QString &brText = opening ? br.opening : br.closing;
        if (brText.isEmpty())
            continue;
        int len = brText.length();
        int pos = opening ? posInBlock : posInBlock - len;
        if (text.mid(pos, len) != brText)
            continue;
        if (br.escape.isEmpty())
        {
            bracket = &br;
            return true;
        }
        len = br.escape.length();
        if (text.mid(pos - len, len) != br.escape)
        {
            bracket = &br;
            return true;
        }
    }
    return false;
}

/*============================== Public slots ==============================*/

void BAbstractCodeEditorDocumentPrivate::loadingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                         bool success, const QString &text)
{
    if (operation.document != q_func())
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>(sender());
    disconnect(driver, SIGNAL(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)),
               this, SLOT(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)));
    if (success)
    {
        if (!operation.fileName.isEmpty())
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        q_func()->setText(text, asyncMin);
        q_func()->setModification(true);
        q_func()->clearUndoRedoStacks(QTextDocument::UndoAndRedoStacks);
    }
    QMetaObject::invokeMethod(q_func(), "loadingFinished", Q_ARG(bool, success));
    setBuisy(false);
}

void BAbstractCodeEditorDocumentPrivate::savingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                        bool success)
{
    if (operation.document != q_func())
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>(sender());
    disconnect(driver, SIGNAL(savingFinished(BAbstractDocumentDriver::Operation, bool)),
               this, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool)));
    if (success)
    {
        if (!operation.fileName.isEmpty())
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        q_func()->setModification(false);
    }
    QMetaObject::invokeMethod(q_func(), "savingFinished", Q_ARG(bool, success));
    setBuisy(false);
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

/*============================================================================
================================ BAbstractCodeEditorDocument =================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractCodeEditorDocument::BAbstractCodeEditorDocument(BCodeEditor *editor, QWidget *parent) :
    QWidget(parent), BBase(*new BAbstractCodeEditorDocumentPrivate(this, editor))
{
    d_func()->init();
}

BAbstractCodeEditorDocument::~BAbstractCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractCodeEditorDocument::BAbstractCodeEditorDocument(BAbstractCodeEditorDocumentPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BAbstractCodeEditorDocument::init()
{
    if (d_func()->edit)
        return;
    d_func()->createEdit();
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
    highlightBrackets();
}

void BAbstractCodeEditorDocument::setBracketHighlightingEnabled(bool enabled)
{
    if (enabled == d_func()->bracketsHighlighting)
        return;
    d_func()->bracketsHighlighting = enabled;
    highlightBrackets();
}

void BAbstractCodeEditorDocument::setSpellChecker(BSpellChecker *sc)
{
    d_func()->spellChecker = sc;
    d_func()->highlighter->rehighlight();
}

void BAbstractCodeEditorDocument::setFileName(const QString &fn)
{
    if (d_func()->buisy)
        return;
    d_func()->setFileName(fn);
}

void BAbstractCodeEditorDocument::setCodec(QTextCodec *codec)
{
    if (d_func())
        return;
    d_func()->setCodec(codec);
}

void BAbstractCodeEditorDocument::setCodec(const QString &codecName)
{
    setCodec(BeQt::codec(codecName));
}

void BAbstractCodeEditorDocument::setAsyncProcessingMinimumLength(int length)
{
    d_func()->asyncMin = (length > 0) ? length : 0;
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
    if (!driver->load(this, codec, fileName))
    {
        disconnect(driver, SIGNAL(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)),
                   d, SLOT(loadingFinished(BAbstractDocumentDriver::Operation, bool, QString)));
        d->setBuisy(false);
        return false;
    }
    return true;
}

bool BAbstractCodeEditorDocument::save(BAbstractDocumentDriver *driver, const QString &fileName)
{
    return save(driver, 0, fileName);
}

bool BAbstractCodeEditorDocument::save(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName)
{
    if (!driver || isBuisy())
        return false;
    B_D(BAbstractCodeEditorDocument);
    d->setBuisy(true);
    connect(driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool)),
            d, SLOT(savingFinished(BAbstractDocumentDriver::Operation, bool)));
    if (!driver->save(this, codec, fileName))
    {
        disconnect(driver, SIGNAL(savingFinished(BAbstractDocumentDriver::Operation, bool)),
                   d, SLOT(savingFinished(BAbstractDocumentDriver::Operation, bool)));
        d->setBuisy(false);
        return false;
    }
    return true;
}

bool BAbstractCodeEditorDocument::waitForProcessed(int msecs)
{
    B_D(BAbstractCodeEditorDocument);
    if (!d->buisy)
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(buisyChanged(bool)), msecs);
    return !d->buisy;
}

bool BAbstractCodeEditorDocument::isReadOnly() const
{
    return d_func()->readOnly;
}

bool BAbstractCodeEditorDocument::isModified() const
{
    return d_func()->isModified;
}

bool BAbstractCodeEditorDocument::hasSelection() const
{
    return d_func()->hasSelection;
}

bool BAbstractCodeEditorDocument::isCutAvailable() const
{
    return d_func()->cutAvailable;
}

bool BAbstractCodeEditorDocument::isCopyAvailable() const
{
    return d_func()->copyAvailable;
}

bool BAbstractCodeEditorDocument::isPasteAvailable() const
{
    return d_func()->pasteAvailable;
}

bool BAbstractCodeEditorDocument::isUndoAvailable() const
{
    return d_func()->undoAvailable;
}

bool BAbstractCodeEditorDocument::isRedoAvailable() const
{
    return d_func()->redoAvailable;
}

QPoint BAbstractCodeEditorDocument::cursorPosition() const
{
    return d_func()->cursorPosition;
}

BAbstractFileType *BAbstractCodeEditorDocument::fileType() const
{
    return d_func()->fileType;
}

QString BAbstractCodeEditorDocument::fileTypeId() const
{
    return d_func()->fileType ? d_func()->fileType->id() : QString();
}

BAbstractCodeEditorDocument::BracketPairList BAbstractCodeEditorDocument::recognizedBrackets() const
{
    return d_func()->recognizedBrackets;
}

bool BAbstractCodeEditorDocument::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

BSpellChecker *BAbstractCodeEditorDocument::spellChecker() const
{
    return d_func()->spellChecker;
}

QString BAbstractCodeEditorDocument::fileName() const
{
    return d_func()->fileName;
}

QTextCodec *BAbstractCodeEditorDocument::codec() const
{
    return d_func()->codec;
}

QString BAbstractCodeEditorDocument::codecName() const
{
    return BeQt::codecName(d_func()->codec);
}

int BAbstractCodeEditorDocument::asyncProcessingMinimumLength() const
{
    return d_func()->asyncMin;
}

bool BAbstractCodeEditorDocument::isBuisy() const
{
    return d_func()->buisy;
}

BCodeEditor *BAbstractCodeEditorDocument::editor() const
{
    return d_func()->Editor;
}

/*============================== Public slots ==============================*/

void BAbstractCodeEditorDocument::setFocus()
{
    if (d_func()->edit)
        setFocusImplementation();
}

void BAbstractCodeEditorDocument::activateWindow()
{
    if (d_func()->edit)
        activateWindowImplementation();
}

void BAbstractCodeEditorDocument::setText(const QString &txt, int asyncIfLongerThan)
{
    setTextImplementation(txt, asyncIfLongerThan);
}

void BAbstractCodeEditorDocument::insertText(const QString &txt)
{
    insertTextImplementation(txt);
}

void BAbstractCodeEditorDocument::moveCursor(const QPoint &pos)
{
    moveCursorImplementation(pos);
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::selectText(const QPoint &start, const QPoint &end)
{
    selectTextImplementation(start, end);
}

void BAbstractCodeEditorDocument::selectText(int start, int end)
{
    selectTextImplementation(start, end);
}

void BAbstractCodeEditorDocument::selectLines(int firstLine, int lastLine)
{
    selectLinesImplementation(firstLine, lastLine);
}

void BAbstractCodeEditorDocument::selectAll()
{
    selectAllImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::deselectText()
{
    deselectTextImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::cut()
{
    cutImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::copy()
{
    copyImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::paste()
{
    pasteImplementation();
}

void BAbstractCodeEditorDocument::deleteSelection()
{
    deleteSelectionImplementation();
    setFocusImplementation();
}

void BAbstractCodeEditorDocument::undo()
{
    undoImplementation();
    setFocusImplementation();
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

/*============================== Protected methods =========================*/

void BAbstractCodeEditorDocument::highlightBrackets()
{
    d_func()->highlightBrackets();
}

QTextCursor BAbstractCodeEditorDocument::textCursor(bool *ok) const
{
    if (ok)
        *ok = true;
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
        return bRet(ok, false, QTextCursor());
}

QMenu *BAbstractCodeEditorDocument::createSpellCheckerMenu(const QPoint &pos) const
{
    if (!d_func()->spellChecker)
        return 0;
    QTextEdit *tedt = qobject_cast<QTextEdit *>(innerEdit());
    QPlainTextEdit *ptedt = qobject_cast<QPlainTextEdit *>(innerEdit());
    BCodeEdit *cedt = qobject_cast<BCodeEdit *>(innerEdit());
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
    d_func()->wordToReplace.first = tc.block().position() + i + 1;
    i = p + 1;
    while (i < text.length() && (text.at(i).isLetterOrNumber() || text.at(i) == '_'))
        w.append(text.at(i++));
    d_func()->wordToReplace.second = d_func()->wordToReplace.first + w.length();
    if (w.isEmpty())
        return 0;
    QMenu *mnu = new QMenu(tr("Spell check", "mnu title"));
    mnu->setEnabled(!d_func()->readOnly);
    QStringList suggestions = d_func()->spellChecker->suggest(w);
    while (suggestions.size() > 10)
        suggestions.removeLast();
    foreach (const QString &s, suggestions)
        mnu->addAction(s, d_func(), SLOT(replaceWord()));
    if (!suggestions.isEmpty())
        mnu->addSeparator();
    if (spellChecker()->isIgnored(w))
        mnu->addAction(tr("Remove from ignore list", "act text"), d_func(),
                       SLOT(dontIgnoreWord()))->setProperty("beqt/word", w);
    else if (!d_func()->spellChecker->spell(w))
        mnu->addAction(tr("Ignore this word", "act text"), d_func(), SLOT(ignoreWord()))->setProperty("beqt/word", w);
    return mnu;
}

void BAbstractCodeEditorDocument::clearUndoRedoStacks(QTextDocument::Stacks historyToClear)
{
    QTextDocument *doc = innerDocument();
    if (!doc)
        return;
    doc->clearUndoRedoStacks(historyToClear);
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

QWidget *BAbstractCodeEditorDocument::innerEdit(QTextDocument **doc) const
{
    if (doc)
        *doc = d_func()->highlighter ? d_func()->highlighter->document() : 0;
    return d_func()->edit;
}

QTextDocument *BAbstractCodeEditorDocument::innerDocument() const
{
    return d_func()->highlighter ? d_func()->highlighter->document() : 0;
}

/*============================== Protected slots ===========================*/

void BAbstractCodeEditorDocument::emitSelectionChanged()
{
    Q_EMIT selectionChanged();
}

void BAbstractCodeEditorDocument::setReadOnlyInternal(bool ro)
{
    if (ro == d_func()->readOnly)
        return;
    d_func()->readOnly = ro;
    Q_EMIT readOnlyChanged(ro);
}

void BAbstractCodeEditorDocument::setModificationInternal(bool modified)
{
    if (modified == d_func()->isModified)
        return;
    d_func()->isModified = modified;
    Q_EMIT modificationChanged(modified);
}

void BAbstractCodeEditorDocument::setHasSelection(bool hasSelection)
{
    if (hasSelection == d_func()->hasSelection)
        return;
    d_func()->hasSelection = hasSelection;
    Q_EMIT hasSelectionChanged(hasSelection);
}

void BAbstractCodeEditorDocument::setCutAvailable(bool available)
{
    if (available == d_func()->cutAvailable)
        return;
    d_func()->cutAvailable = available;
    Q_EMIT cutAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setCopyAvailable(bool available)
{
    if (available == d_func()->copyAvailable)
        return;
    d_func()->copyAvailable = available;
    Q_EMIT copyAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setPasteAvailable(bool available)
{
    if (available == d_func()->pasteAvailable)
        return;
    d_func()->pasteAvailable = available;
    Q_EMIT pasteAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setUndoAvailable(bool available)
{
    if (available == d_func()->undoAvailable)
        return;
    d_func()->undoAvailable = available;
    Q_EMIT undoAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setRedoAvailable(bool available)
{
    if (available == d_func()->redoAvailable)
        return;
    d_func()->redoAvailable = available;
    Q_EMIT redoAvailableChanged(available);
}

void BAbstractCodeEditorDocument::setCursorPosition(const QPoint &pos)
{
    if (pos == d_func()->cursorPosition)
        return;
    d_func()->cursorPosition = pos;
    highlightBrackets();
    Q_EMIT cursorPositionChanged(pos);
}

void BAbstractCodeEditorDocument::setBuisy(bool buisy)
{
    Q_EMIT buisyChanged(buisy);
}
