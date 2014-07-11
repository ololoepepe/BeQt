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

#ifndef BABSTRACTCODEEDITORDOCUMENT_H
#define BABSTRACTCODEEDITORDOCUMENT_H

class BAbstractCodeEditorDocumentPrivate;

class BAbstractDocumentDriver;
class BCodeEdit;
class BCodeEditor;
class BCodeEditorPrivate;
class BSpellChecker;

class QFont;
class QMenu;
class QPoint;
class QRegExp;
class QTextCodec;
class QTextCursor;

#include "babstractfiletype.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QList>
#include <QString>
#include <QTextDocument>
#include <QVariant>
#include <QWidget>

/*============================================================================
================================ BAbstractCodeEditorDocument =================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractCodeEditorDocument : public QWidget, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAbstractCodeEditorDocument)
public:
    struct TextProcessingResult
    {
        QString text;
        QVariant userData;
    };
public:
    typedef BAbstractFileType::BracketPair BracketPair;
    typedef QList<BracketPair> BracketPairList;
    typedef TextProcessingResult (*TextProcessingFunction)(const QString &text, const QVariant &userData);
public:
    explicit BAbstractCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BAbstractCodeEditorDocument();
protected:
    explicit BAbstractCodeEditorDocument(BAbstractCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    int asyncProcessingMinimumLength() const;
    QTextCodec *codec() const;
    QString codecName() const;
    int cursorPosition() const;
    QPoint cursorPositionRowColumn() const;
    virtual QFont editFont() const = 0;
    BCodeEditor *editor() const;
    virtual BeQt::TabWidth editTabWidth() const = 0;
    QString fileName() const;
    BAbstractFileType *fileType() const;
    QString fileTypeId() const;
    virtual bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true) = 0;
    virtual bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true) = 0;
    bool hasSelection() const;
    void init();
    virtual void installInnerEventFilter(QObject *filter) = 0;
    bool isBracketHighlightingEnabled() const;
    bool isBuisy() const;
    bool isCopyAvailable() const;
    bool isCutAvailable() const;
    virtual bool isEditAutoIndentationEnabled() const = 0;
    virtual bool isLineNumberWidgetVisible() const = 0;
    bool isModified() const;
    bool isPasteAvailable() const;
    bool isReadOnly() const;
    bool isRedoAvailable() const;
    bool isUndoAvailable() const;
    bool load(BAbstractDocumentDriver *driver, const QString &fileName = QString());
    bool load(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString());
    BracketPairList recognizedBrackets() const;
    virtual void removeInnerEventFilter(QObject *filter) = 0;
    virtual int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0) = 0;
    virtual int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText) = 0;
    virtual int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0) = 0;
    virtual int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText) = 0;
    virtual bool replaceNext(const QString &newText) = 0;
    bool save(BAbstractDocumentDriver *driver, const QString &fileName = QString());
    bool save(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString());
    virtual QString selectedText(bool full = false) const = 0;
    virtual int selectionEnd() const = 0;
    virtual QPoint selectionEndRowColumn() const = 0;
    virtual int selectionStart() const = 0;
    virtual QPoint selectionStartRowColumn() const = 0;
    void setAsyncProcessingMinimumLength(int length);
    void setBracketHighlightingEnabled(bool enabled);
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    virtual void setEditAutoIndentationEnabled(bool enabled) = 0;
    virtual void setEditFont(const QFont &fnt) = 0;
    virtual void setEditTabWidth(BeQt::TabWidth tw) = 0;
    void setFileName(const QString &fn);
    void setFileType(BAbstractFileType *ft);
    virtual void setLineNumberWidgetVisible(bool b) = 0;
    virtual void setModification(bool modified) = 0;
    virtual void setReadOnly(bool ro) = 0;
    void setRecognizedBrackets(const BracketPairList &list);
    void setSpellChecker(BSpellChecker *sc);
    BSpellChecker *spellChecker() const;
    virtual QString text(bool full = false) const = 0;
public Q_SLOTS:
    void activateWindow();
    void clear();
    void copy();
    void cut();
    void deleteSelection();
    void deselectText();
    void insertText(const QString &txt);
    void moveCursor(int pos);
    void moveCursor(const QPoint &pos);
    void paste();
    void redo();
    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);
    void selectAll();
    void selectLines(int firstLine, int lastLine);
    void selectText(const QPoint &start, const QPoint &end);
    void selectText(int start, int end);
    void setFocus();
    void setText(const QString &txt);
    void undo();
protected:
    virtual void activateWindowImplementation() = 0;
    virtual void afterPreprocessing(const QVariant &result);
    void blockHighlighter(bool block);
    virtual void clearImplementation() = 0;
    virtual void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    virtual void copyImplementation() = 0;
    virtual QMenu *createContextMenu();
    virtual QWidget *createEdit(QTextDocument **doc = 0) = 0;
    virtual int cursorPositionForRowColumn(const QPoint &pos) const = 0;
    virtual QPoint cursorPositionRowColumnImplementation() const = 0;
    virtual QRect cursorRect(bool *ok = 0) const;
    virtual void cutImplementation() = 0;
    virtual void deleteSelectionImplementation() = 0;
    virtual void deselectTextImplementation() = 0;
    virtual void highlightBrackets();
    QTextDocument *innerDocument() const;
    QWidget *innerEdit(QTextDocument **doc = 0) const;
    virtual void insertTextImplementation(const QString &txt) = 0;
    virtual void installDropHandler(QObject *handler) = 0;
    virtual void moveCursorImplementation(int pos) = 0;
    virtual void moveCursorImplementation(const QPoint &pos) = 0;
    virtual void pasteImplementation() = 0;
    virtual void redoImplementation() = 0;
    virtual void selectAllImplementation() = 0;
    virtual void selectLinesImplementation(int firstLine, int lastLine) = 0;
    virtual void selectTextImplementation(const QPoint &start, const QPoint &end) = 0;
    virtual void selectTextImplementation(int start, int end) = 0;
    virtual void setFocusImplementation() = 0;
    virtual void setTextImplementation(const QString &txt) = 0;
    bool shouldProcessAsynchronously(const QString &txt) const;
    virtual QTextCursor textCursor(bool *ok = 0) const;
    virtual TextProcessingFunction textPreprocessingFunction() const;
    virtual QVariant textPreprocessingUserData();
    virtual void undoImplementation() = 0;
protected Q_SLOTS:
    void emitSelectionChanged();
    void emitTextChanged();
    void setBuisy(bool buisy);
    void setCopyAvailable(bool available);
    void setCursorPosition(int pos);
    void setCursorPosition(const QPoint &pos);
    void setCutAvailable(bool available);
    void setHasSelection(bool hasSelection);
    void setModificationInternal(bool modified);
    void setPasteAvailable(bool available);
    void setReadOnlyInternal(bool ro);
    void setRedoAvailable(bool available);
    void setUndoAvailable(bool available);
Q_SIGNALS:
    void allProcessingFinished();
    void buisyChanged(bool buisy);
    void codecChanged(const QString &codecName);
    void copyAvailableChanged(bool available);
    void cursorPositionChanged(int pos);
    void cursorPositionChanged(const QPoint &pos);
    void cutAvailableChanged(bool available);
    void fileNameChanged(const QString &fn);
    void fileTypeChanged(BAbstractFileType *ft);
    void hasSelectionChanged(bool hasSelection);
    void loadingFinished(bool success);
    void modificationChanged(bool modified);
    void pasteAvailableChanged(bool available);
    void readOnlyChanged(bool ro);
    void redoAvailableChanged(bool available);
    void savingFinished(bool success);
    void selectionChanged();
    void textChanged();
    void undoAvailableChanged(bool available);
private:
    Q_DISABLE_COPY(BAbstractCodeEditorDocument)
    friend class BCodeEditorPrivate;
    friend class BCodeEditPrivate;
};

#endif // BABSTRACTCODEEDITORDOCUMENT_H

