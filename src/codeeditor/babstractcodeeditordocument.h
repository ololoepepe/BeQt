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

class QMenu;
class QRegExp;
class QTextCodec;

#include "babstractfiletype.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QFont>
#include <QList>
#include <QPoint>
#include <QString>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QVariant>
#include <QWidget>

/*============================================================================
================================ BAbstractCodeEditorDocument =================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractCodeEditorDocument : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(int asyncProcessingMinimumLength READ asyncProcessingMinimumLength
               WRITE setAsyncProcessingMinimumLength)
    Q_PROPERTY(bool bracketHighlightingEnabled READ isBracketHighlightingEnabled WRITE setBracketHighlightingEnabled)
    Q_PROPERTY(bool buisy READ isBuisy NOTIFY buisyChanged STORED false)
    Q_PROPERTY(QTextCodec * codec READ codec WRITE setCodec)
    Q_PROPERTY(QString codecName READ codecName WRITE setCodec NOTIFY codecChanged)
    Q_PROPERTY(bool copyAvailable READ isCopyAvailable NOTIFY copyAvailableChanged STORED false)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE moveCursor NOTIFY cursorPositionChanged)
    Q_PROPERTY(QPoint cursorPositionRowColumn READ cursorPositionRowColumn WRITE moveCursor
               NOTIFY cursorPositionChanged)
    Q_PROPERTY(QRect cursorRect READ cursorRect STORED false)
    Q_PROPERTY(bool cutAvailable READ isCutAvailable NOTIFY cutAvailableChanged STORED false)
    Q_PROPERTY(bool editAutoIndentationEnabled READ isEditAutoIndentationEnabled WRITE setEditAutoIndentationEnabled)
    Q_PROPERTY(QFont editFont READ editFont WRITE setEditFont)
    Q_PROPERTY(BCodeEditor * editor READ editor CONSTANT)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(BAbstractFileType * fileType READ fileType WRITE setFileType NOTIFY fileTypeChanged)
    Q_PROPERTY(QString fileTypeId READ fileTypeId STORED false)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged STORED false)
    Q_PROPERTY(QTextDocument * innerDocument READ innerDocument CONSTANT)
    Q_PROPERTY(bool lineNumberWidgetVisible READ isLineNumberWidgetVisible WRITE setLineNumberWidgetVisible)
    Q_PROPERTY(bool modified READ isModified NOTIFY modificationChanged STORED false)
    Q_PROPERTY(bool pasteAvailable READ isPasteAvailable NOTIFY pasteAvailableChanged STORED false)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool redoAvailable READ isRedoAvailable NOTIFY redoAvailableChanged STORED false)
    Q_PROPERTY(int selectionEnd READ selectionEnd STORED false)
    Q_PROPERTY(QPoint selectionEndRowColumn READ selectionEndRowColumn STORED false)
    Q_PROPERTY(int selectionStart READ selectionStart STORED false)
    Q_PROPERTY(QPoint selectionStartRowColumn READ selectionStartRowColumn STORED false)
    Q_PROPERTY(BSpellChecker * spellChecker READ spellChecker WRITE setSpellChecker)
    Q_PROPERTY(QTextCursor textCursor READ textCursor STORED false)
    Q_PROPERTY(bool undoAvailable READ isUndoAvailable NOTIFY undoAvailableChanged STORED false)
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
    typedef QTextEdit::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
    typedef TextProcessingResult (*TextProcessingFunction)(const QString &text, const QVariant &userData);
public:
    explicit BAbstractCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BAbstractCodeEditorDocument();
protected:
    explicit BAbstractCodeEditorDocument(BAbstractCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    int asyncProcessingMinimumLength() const;
    void blockHighlighter(bool block);
    QTextCodec *codec() const;
    QString codecName() const;
    virtual QMenu *createContextMenu();
    ExtraSelection createExtraSelection(const QTextCharFormat &format = QTextCharFormat()) const;
    int cursorPosition() const;
    virtual int cursorPositionForRowColumn(const QPoint &pos) const = 0;
    QPoint cursorPositionRowColumn() const;
    virtual QRect cursorRect() const;
    virtual QFont editFont() const = 0;
    BCodeEditor *editor() const;
    virtual BeQt::TabWidth editTabWidth() const = 0;
    ExtraSelectionList extraSelections() const;
    QString fileName() const;
    BAbstractFileType *fileType() const;
    QString fileTypeId() const;
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool hasSelection() const;
    void init();
    QTextDocument *innerDocument() const;
    QWidget *innerEdit(QTextDocument **doc = 0) const;
    virtual void installInnerEventFilter(QObject *filter) = 0;
    virtual void installInnerViewportEventFilter(QObject *filter) = 0;
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
    bool save(BAbstractDocumentDriver *driver, QTextCodec *codec, BeQt::LineFeed lineFeed,
              const QString &fileName = QString());
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
    void setExtraSelections(const ExtraSelectionList &list);
    void setFileName(const QString &fn);
    void setFileType(BAbstractFileType *ft);
    virtual void setLineNumberWidgetVisible(bool b) = 0;
    virtual void setModification(bool modified) = 0;
    virtual void setReadOnly(bool ro) = 0;
    void setRecognizedBrackets(const BracketPairList &list);
    void setSpellChecker(BSpellChecker *sc);
    bool shouldProcessAsynchronously(const QString &txt) const;
    BSpellChecker *spellChecker() const;
    virtual QString text(bool full = false) const = 0;
    virtual QTextCursor textCursor() const;
    virtual QTextCursor textCursorForPosition(const QPoint &pos) const;
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
    virtual void clearImplementation() = 0;
    virtual void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    virtual void copyImplementation() = 0;
    virtual QWidget *createEdit(QTextDocument **doc = 0) = 0;
    virtual QPoint cursorPositionRowColumnImplementation() const = 0;
    virtual void cutImplementation() = 0;
    virtual void deleteSelectionImplementation() = 0;
    virtual void deselectTextImplementation() = 0;
    virtual bool findNextImplementation(const QString &txt, QTextDocument::FindFlags flags = 0,
                                        bool cyclic = true) = 0;
    virtual bool findNextRegexpImplementation(const QRegExp &rx, QTextDocument::FindFlags flags = 0,
                                              bool cyclic = true) = 0;
    virtual void insertTextImplementation(const QString &txt) = 0;
    virtual void moveCursorImplementation(int pos) = 0;
    virtual void moveCursorImplementation(const QPoint &pos) = 0;
    virtual void pasteImplementation() = 0;
    virtual void redoImplementation() = 0;
    virtual void selectAllImplementation() = 0;
    virtual void selectLinesImplementation(int firstLine, int lastLine) = 0;
    virtual void selectTextImplementation(const QPoint &start, const QPoint &end) = 0;
    virtual void selectTextImplementation(int start, int end) = 0;
    virtual void setExtraSelectionsImplementation(const ExtraSelectionList &list) = 0;
    virtual void setFocusImplementation() = 0;
    virtual void setTextImplementation(const QString &txt) = 0;
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

