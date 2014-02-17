#include "bsimplecodeeditordocument.h"
#include "bsimplecodeeditordocument_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQt>
#include <BeQtWidgets/BClipboardNotifier>
#include <BeQtWidgets/BLineNumberWidget>
#include <BeQtCore/BTextTools>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QTextDocument>
#include <QColor>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QList>
#include <QChar>
#include <QPoint>
#include <QTextBlock>
#include <QVariant>
#include <QVariantMap>

#include <QDebug>

/*static BAbstractCodeEditorDocument::TextProcessingResult procFunc(const QString &text, const QVariantMap &m)
{
    BAbstractCodeEditorDocument::TextProcessingResult r;
    r.text = text;
    //r.text.remove();
    BTextTools::replaceTabs(&r.text, static_cast<BeQt::TabWidth>(m.value("tab_width").toInt()));
    return r;
}*/

/*============================================================================
================================ BSimpleCodeEditorDocumentPrivate ============
============================================================================*/

/*============================== Public constructors =======================*/

BSimpleCodeEditorDocumentPrivate::BSimpleCodeEditorDocumentPrivate(BSimpleCodeEditorDocument *q, BCodeEditor *editor) :
    BAbstractCodeEditorDocumentPrivate(q, editor)
{
    //
}

BSimpleCodeEditorDocumentPrivate::~BSimpleCodeEditorDocumentPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSimpleCodeEditorDocumentPrivate::init()
{
    ptedt = 0;
}

QWidget *BSimpleCodeEditorDocumentPrivate::createEdit(QTextDocument **doc)
{
    if (!BClipboardNotifier::instance())
        new BClipboardNotifier;
    connect(BClipboardNotifier::instance(), SIGNAL(textDataAvailableChanged(bool)),
            this, SLOT(updatePasteAvailable(bool)));
    B_Q(BSimpleCodeEditorDocument);
    tabWidth = BeQt::TabWidth4;
    ptedt = new QPlainTextEdit;
      ptedt->setLineWrapMode(QPlainTextEdit::NoWrap);
      ptedt->setWordWrapMode(QTextOption::NoWrap);
      ptedt->installEventFilter(this);
    lnwgt = new BLineNumberWidget(ptedt);
    bool hasSel = ptedt->textCursor().hasSelection();
    q->setReadOnlyInternal(ptedt->isReadOnly());
    q->setModificationInternal(ptedt->document()->isModified());
    q->setHasSelection(hasSel);
    q->setCutAvailable(hasSel && !ptedt->isReadOnly());
    q->setCopyAvailable(hasSel);
    q->setPasteAvailable(!ptedt->isReadOnly() && BClipboardNotifier::instance()->textDataAvailable());
    q->setUndoAvailable(ptedt->document()->isUndoAvailable());
    q->setRedoAvailable(ptedt->document()->isRedoAvailable());
    q->setCursorPosition(QPoint());
    q->setBuisy(false);
    connect(ptedt, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(ptedt, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(ptedt, SIGNAL(selectionChanged()), q, SLOT(emitSelectionChanged()));
    connect(ptedt, SIGNAL(modificationChanged(bool)), q, SLOT(setModificationInternal(bool)));
    connect(ptedt, SIGNAL(undoAvailable(bool)), q, SLOT(setUndoAvailable(bool)));
    connect(ptedt, SIGNAL(redoAvailable(bool)), q, SLOT(setRedoAvailable(bool)));
    if (doc)
        *doc = ptedt->document();
    return ptedt;
}

bool BSimpleCodeEditorDocumentPrivate::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type())
    {
    case QEvent::KeyPress:
        return keyPressEvent(static_cast<QKeyEvent *>(e));
    default:
        return QObject::eventFilter(obj, e);
    }
}

bool BSimpleCodeEditorDocumentPrivate::keyPressEvent(QKeyEvent *e)
{
    int modifiers = e->modifiers();
    int key = e->key();
    switch (modifiers)
    {
    case Qt::NoModifier:
        switch (key)
        {
        case Qt::Key_Tab:
            handleTab();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void BSimpleCodeEditorDocumentPrivate::handleTab()
{
    if (ptedt->isReadOnly())
        return;
    QTextCursor tc = ptedt->textCursor();
    tc = ptedt->textCursor();
    int posb = tc.positionInBlock();
    int spcount = tabWidth - (posb < tabWidth ? posb : posb % tabWidth);
    if (!spcount)
        spcount = tabWidth;
    q_func()->insertText(QString().fill(' ', spcount));
}

/*============================== Public slots ==============================*/

void BSimpleCodeEditorDocumentPrivate::cursorPositionChanged()
{
    QTextCursor tc = ptedt->textCursor();
    q_func()->setCursorPosition(QPoint(tc.positionInBlock(), tc.blockNumber()));
}

void BSimpleCodeEditorDocumentPrivate::selectionChanged()
{
    bool hasSel = ptedt->textCursor().hasSelection();
    q_func()->setHasSelection(hasSel);
    q_func()->setCutAvailable(hasSel && !ptedt->isReadOnly());
    q_func()->setCopyAvailable(hasSel);
}

void BSimpleCodeEditorDocumentPrivate::updatePasteAvailable(bool available)
{
    q_func()->setPasteAvailable(available && !ptedt->isReadOnly());
}

/*============================================================================
================================ BSimpleCodeEditorDocument ===================
============================================================================*/

/*============================== Public constructors =======================*/

BSimpleCodeEditorDocument::BSimpleCodeEditorDocument(BCodeEditor *editor, QWidget *parent) :
    BAbstractCodeEditorDocument(*new BSimpleCodeEditorDocumentPrivate(this, editor), parent)
{
    d_func()->init();
}

BSimpleCodeEditorDocument::~BSimpleCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BSimpleCodeEditorDocument::BSimpleCodeEditorDocument(BSimpleCodeEditorDocumentPrivate &d, QWidget *parent) :
    BAbstractCodeEditorDocument(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BSimpleCodeEditorDocument::setReadOnly(bool ro)
{
    bool b = d_func()->ptedt->isReadOnly() != ro;
    d_func()->ptedt->setReadOnly(ro);
    if (b)
        setReadOnlyInternal(ro);
}

void BSimpleCodeEditorDocument::setModification(bool modified)
{
    d_func()->ptedt->document()->setModified(modified);
}

void BSimpleCodeEditorDocument::setEditFont(const QFont &fnt)
{
    d_func()->ptedt->setFont(fnt);
}

void BSimpleCodeEditorDocument::setEditTabWidth(BeQt::TabWidth tw)
{
    d_func()->tabWidth = tw;
}

void BSimpleCodeEditorDocument::setLineNumberWidgetVisible(bool b)
{
    d_func()->lnwgt->setVisible(b);
}

bool BSimpleCodeEditorDocument::findNext(const QString &txt, QTextDocument::FindFlags flags, bool cyclic)
{
    bool b = d_func()->ptedt->find(txt, flags);
    if (!b && cyclic)
    {
        if (flags & QTextDocument::FindBackward)
            d_func()->ptedt->moveCursor(QTextCursor::End);
        else
            d_func()->ptedt->moveCursor(QTextCursor::Start);
        b = d_func()->ptedt->find(txt, flags);
    }
    return b;
}

bool BSimpleCodeEditorDocument::replaceNext(const QString &newText)
{
    if (isReadOnly() || !hasSelection())
        return false;
    insertText(newText);
    return true;
}

int BSimpleCodeEditorDocument::replaceInSelection(const QString &text, const QString &newText, Qt::CaseSensitivity cs)
{
    if (isReadOnly() || !hasSelection() || text.isEmpty() || text == newText)
        return -1;
    B_D(BSimpleCodeEditorDocument);
    QString ntext = selectedText(true);
    int count = ntext.count(text, cs);
    if (!count)
        return 0;
    QTextCursor tc = d->ptedt->textCursor();
    tc.beginEditBlock();
    int start = tc.selectionStart();
    int end = tc.selectionEnd();
    int len = ntext.length();
    ntext.replace(text, newText, cs);
    insertText(ntext);
    if (start < end)
        end += (ntext.length() - len);
    else
        start += (ntext.length() - len);
    selectText(start, end);
    tc.endEditBlock();
    QTextBlock tb = d->ptedt->document()->findBlock(qMin<int>(start, end));
    QTextBlock tbe = d->ptedt->document()->findBlock(qMax<int>(start, end));
    while (tb.isValid() && tb.blockNumber() < tbe.blockNumber())
    {
        rehighlightBlock(tb);
        tb = tb.next();
    }
    return count;
}

int BSimpleCodeEditorDocument::replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs)
{
    if (isReadOnly() || txt.isEmpty() || txt == newText)
        return -1;
    QString ptext = text(true);
    int count = ptext.count(txt, cs);
    if (!count)
        return 0;
    int find = ptext.indexOf(txt, 0, cs);
    int lind = ptext.lastIndexOf(txt, -1, cs) + txt.length();
    selectText(find, lind);
    insertText(ptext.mid(find, lind - find).replace(txt, newText, cs));
    return count;
}

QFont BSimpleCodeEditorDocument::editFont() const
{
    return d_func()->ptedt->font();
}

BeQt::TabWidth BSimpleCodeEditorDocument::editTabWidth() const
{
    return d_func()->tabWidth;
}

bool BSimpleCodeEditorDocument::lineNumberWidgetVisible() const
{
    return d_func()->lnwgt->isVisible();
}

QString BSimpleCodeEditorDocument::text(bool) const
{
    return d_func()->ptedt->toPlainText().replace(QChar::ParagraphSeparator, '\n');
}

QString BSimpleCodeEditorDocument::selectedText(bool) const
{
    return d_func()->ptedt->textCursor().selectedText().replace(QChar::ParagraphSeparator, '\n');
}

QPoint BSimpleCodeEditorDocument::selectionStart() const
{
    if (!hasSelection())
        return cursorPosition();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionStart());
    return QPoint(tc.selectionStart() - tb.position(), tb.blockNumber());
}

QPoint BSimpleCodeEditorDocument::selectionEnd() const
{
    if (!hasSelection())
        return cursorPosition();
    const B_D(BSimpleCodeEditorDocument);
    QTextCursor tc = d->ptedt->textCursor();
    QTextBlock tb = d->ptedt->document()->findBlock(tc.selectionEnd());
    return QPoint(tc.selectionEnd() - tb.position(), tb.blockNumber());
}

/*============================== Protected methods =========================*/

QWidget *BSimpleCodeEditorDocument::createEdit(QTextDocument **doc)
{
    return d_func()->createEdit(doc);
}

void BSimpleCodeEditorDocument::setFocusImplementation()
{
    d_func()->ptedt->setFocus();
}

void BSimpleCodeEditorDocument::activateWindowImplementation()
{
    d_func()->ptedt->activateWindow();
}

void BSimpleCodeEditorDocument::setTextImplementation(const QString &txt)
{
    d_func()->ptedt->setPlainText(txt);
}

void BSimpleCodeEditorDocument::insertTextImplementation(const QString &txt)
{
    d_func()->ptedt->insertPlainText(txt);
}

void BSimpleCodeEditorDocument::moveCursorImplementation(const QPoint &pos)
{
    B_D(BSimpleCodeEditorDocument);
    QTextBlock tb = d->ptedt->document()->findBlockByLineNumber(pos.y());
    if (!tb.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tb.position() + (pos.x() > 0 ? pos.x() : 0));
    d->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::selectTextImplementation(const QPoint &start, const QPoint &end)
{
    B_D(BSimpleCodeEditorDocument);
    QTextBlock tbs = d->ptedt->document()->findBlockByLineNumber(start.y());
    if (!tbs.isValid())
        return;
    QTextBlock tbe = d->ptedt->document()->findBlockByLineNumber(end.y());
    if (!tbe.isValid())
        return;
    QTextCursor tc = d->ptedt->textCursor();
    tc.setPosition(tbs.position() + (start.x() > 0 ? start.x() : 0));
    tc.setPosition(tbe.position() + (end.x() > 0 ? end.x() : 0), QTextCursor::KeepAnchor);
    d->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::selectTextImplementation(int start, int end)
{
    if (start < 0 || end < 0)
        return;
    QTextCursor tc = d_func()->ptedt->textCursor();
    tc.setPosition(start);
    tc.setPosition(end, QTextCursor::KeepAnchor);
    d_func()->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::selectLinesImplementation(int firstLine, int lastLine)
{
    int bc = d_func()->ptedt->blockCount();
    if (firstLine < 0 || firstLine >= bc)
        return;
    QPoint start(0, firstLine);
    QPoint end(0, firstLine);
    if (lastLine > 0 && lastLine < bc)
        end.setY(lastLine);
    QTextBlock tb = d_func()->ptedt->document()->findBlockByNumber(lastLine);
    if (tb.isValid())
        end.setX(tb.length() - 1);
    selectText(start, end);
}

void BSimpleCodeEditorDocument::selectAllImplementation()
{
    d_func()->ptedt->selectAll();
}

void BSimpleCodeEditorDocument::deselectTextImplementation()
{
    QTextCursor tc = d_func()->ptedt->textCursor();
    if (!tc.hasSelection())
        return;
    tc.setPosition(tc.selectionEnd());
    d_func()->ptedt->setTextCursor(tc);
}

void BSimpleCodeEditorDocument::cutImplementation()
{
    d_func()->ptedt->cut();
}

void BSimpleCodeEditorDocument::copyImplementation()
{
    d_func()->ptedt->copy();
}

void BSimpleCodeEditorDocument::pasteImplementation()
{
    d_func()->ptedt->paste();
}

void BSimpleCodeEditorDocument::deleteSelectionImplementation()
{
    if (isReadOnly() || !hasSelection())
        return;
    QTextCursor tc = d_func()->ptedt->textCursor();
    tc.beginEditBlock();
    tc.removeSelectedText();
    tc.endEditBlock();
}

void BSimpleCodeEditorDocument::undoImplementation()
{
    d_func()->ptedt->undo();
}

void BSimpleCodeEditorDocument::redoImplementation()
{
    d_func()->ptedt->redo();
}

void BSimpleCodeEditorDocument::installDropHandler(QObject *handler)
{
    d_func()->ptedt->viewport()->installEventFilter(handler);
}

void BSimpleCodeEditorDocument::installInnerEventFilter(QObject *filter)
{
    d_func()->ptedt->installEventFilter(filter);
}

/*BAbstractCodeEditorDocument::TextProcessingFunction BSimpleCodeEditorDocument::textPreprocessingFunction() const
{
    return &procFunc;
}

QVariantMap BSimpleCodeEditorDocument::preprocessingUserData()
{
    QVariantMap m;
    m.insert("tab_width", editTabWidth());
    return m;
}*/
