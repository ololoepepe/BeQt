#ifndef BABSTRACTCODEEDITORDOCUMENT_P_H
#define BABSTRACTCODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BTextBlockUserData;

class QWidget;
class QTextCodec;
class QTextDocument;
class QTextCharFormat;
class QColor;
class QFont;
class QTextBlock;
class QPlainTextEdit;
class QTextCursor;

#include "babstractcodeeditordocument.h"
#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QTextCharFormat>

/*============================================================================
================================ BSyntaxHighlighter ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit BSyntaxHighlighter(BAbstractCodeEditorDocument *doc, QTextDocument *parent);
public:
    void setCurrentBlockState(int newState);
    void setCurrentBlockUserData(BTextBlockUserData *data);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
    QTextBlock currentBlock() const;
    int currentBlockState() const;
    BTextBlockUserData *currentBlockUserData() const;
    QTextCharFormat format(int position) const;
    int previousBlockState() const;
    BAbstractCodeEditorDocument *editorDocument() const;
protected:
    void highlightBlock(const QString &text);
public:
    BAbstractCodeEditorDocument *const EditorDocument;
};

/*============================================================================
================================ BAbstractCodeEditorDocumentPrivate ==========
============================================================================*/

class BAbstractCodeEditorDocumentPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractCodeEditorDocument)
public:
    typedef BAbstractFileType::BracketPair BracketPair;
    typedef QList<BracketPair> BracketPairList;
    typedef BAbstractCodeEditorDocument::FindBracketPairResult FindBracketPairResult;
    typedef BAbstractCodeEditorDocument::FindBracketPairResultList FindBracketPairResultList;
    typedef QTextEdit::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
public:
    static void removeExtraSelections(ExtraSelectionList &from, const ExtraSelectionList &what);
    static QTextCharFormat createBracketsFormat();
    static QTextCharFormat createBracketsErrorFormat();
    static QTextCharFormat createLineFormat(const QColor &c);
    static ExtraSelection createExtraSelection(const QTextEdit *edt,
                                               const QTextCharFormat &format = QTextCharFormat());
    static ExtraSelection createExtraSelection(const QPlainTextEdit *edt,
                                               const QTextCharFormat &format = QTextCharFormat());
    static FindBracketPairResult createFindBracketPairResult();
public:
    explicit BAbstractCodeEditorDocumentPrivate(BAbstractCodeEditorDocument *q, BCodeEditor *editor);
    ~BAbstractCodeEditorDocumentPrivate();
public:
    void init();
    bool createEdit();
    void setBuisy(bool b);
    void setFileName(QString fn);
    void setCodec(QTextCodec *c);
    void rehighlight();
    FindBracketPairResult findLeftBracketPair(QTextEdit *edt) const;
    FindBracketPairResult findLeftBracketPair(QPlainTextEdit *edt) const;
    FindBracketPairResult findRightBracketPair(QTextEdit *edt) const;
    FindBracketPairResult findRightBracketPair(QPlainTextEdit *edt) const;
    bool testBracket(const QString &text, int posInBlock, bool opening, const BracketPair *&bracket) const;
public Q_SLOTS:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
private:
    FindBracketPairResult findRightBracketPairInternal(const QTextCursor &tc) const;
    FindBracketPairResult findLeftBracketPairInternal(const QTextCursor &tc) const;
public:
    BCodeEditor *const Editor;
public:
    QWidget *edit;
    BSyntaxHighlighter *highlighter;
    BAbstractFileType *fileType;
    BracketPairList recognizedBrackets;
    bool bracketsHighlighting;
    QString fileName;
    QTextCodec *codec;
    int asyncMin;
    bool readOnly;
    bool isModified;
    bool hasSelection;
    bool cutAvailable;
    bool copyAvailable;
    bool pasteAvailable;
    bool undoAvailable;
    bool redoAvailable;
    QPoint cursorPosition;
    bool buisy;
private:
    Q_DISABLE_COPY(BAbstractCodeEditorDocumentPrivate)
};

#endif // BABSTRACTCODEEDITORDOCUMENT_P_H

