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

#ifndef BABSTRACTCODEEDITORDOCUMENT_P_H
#define BABSTRACTCODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BTextBlockUserData;
class BSpellChecker;

class QWidget;
class QTextCodec;
class QTextDocument;
class QTextCharFormat;
class QColor;
class QFont;
class QTextBlock;
class QPlainTextEdit;
class QPoint;
class QMenu;

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
#include <QPair>
#include <QFuture>
#include <QFutureWatcher>

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

class B_CODEEDITOR_EXPORT BAbstractCodeEditorDocumentPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractCodeEditorDocument)
public:
    typedef BAbstractFileType::BracketPair BracketPair;
    typedef QList<BracketPair> BracketPairList;
    typedef BAbstractCodeEditorDocument::TextProcessingResult TextProcessingResult;
    typedef QFuture<TextProcessingResult> TextProcessingResultFuture;
    typedef QFutureWatcher<TextProcessingResult> TextProcessingResultFutureWatcher;
public:
    struct FindBracketPairResult
    {
        int start;
        int end;
        const BracketPair *startBr;
        const BracketPair *endBr;
    };
public:
    typedef QList<FindBracketPairResult> FindBracketPairResultList;
    typedef QTextEdit::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
public:
    static void removeExtraSelections(ExtraSelectionList &from, const ExtraSelectionList &what);
    static QTextCharFormat createBracketsFormat();
    static QTextCharFormat createBracketsErrorFormat();
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
    void highlightBrackets();
    void setExtraSelections(const ExtraSelectionList &list, bool *ok = 0);
    ExtraSelectionList getExtraSelections(bool *ok = 0) const;
    ExtraSelection createExtraSelection(const QTextCharFormat &format, bool *ok = 0) const;
    ExtraSelection createExtraSelection(bool *ok = 0) const;
    FindBracketPairResult findLeftBracketPair() const;
    FindBracketPairResult findRightBracketPair() const;
    bool testBracket(const QString &text, int posInBlock, bool opening, const BracketPair *&bracket) const;
    QMenu *createSpellCheckerMenu(const QPoint &pos);
public Q_SLOTS:
    void loadingFinished(const BAbstractDocumentDriver::Operation &operation, bool success, const QString &text);
    void savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success);
    void replaceWord();
    void ignoreWord();
    void dontIgnoreWord();
    void customContextMenuRequested(const QPoint &pos);
    void preprocessingFinished();
public:
    BCodeEditor *const Editor;
public:
    QWidget *edit;
    QTextDocument *document;
    BSyntaxHighlighter *highlighter;
    BAbstractFileType *fileType;
    BracketPairList recognizedBrackets;
    bool bracketsHighlighting;
    ExtraSelectionList highlightedBrackets;
    BSpellChecker *spellChecker;
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
    mutable QPair<int, int> wordToReplace;
private:
    Q_DISABLE_COPY(BAbstractCodeEditorDocumentPrivate)
};

#endif // BABSTRACTCODEEDITORDOCUMENT_P_H

