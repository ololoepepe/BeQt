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

#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;
class BPlainTextEdit;
class BAbstractCodeEditorDocumentPrivate;
class BCodeEditorDocument;
class BCodeEditorDocumentPrivate;

class QString;
class QPoint;
class QFont;
class QTextBlock;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBaseObject>

#include <QWidget>
#include <QList>
#include <QTextDocument>
#include <QTextEdit>

/*============================================================================
================================ BCodeEdit ===================================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEdit : public QWidget, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEdit)

public:
    enum EditMode
    {
        NormalMode,
        BlockMode
    };
public:
    struct SplittedLinesRange
    {
        int firstLineNumber;
        int lastLineNumber;
    };
public:
    typedef QTextEdit::ExtraSelection ExtraSelection;
    typedef QList<ExtraSelection> ExtraSelectionList;
public:
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
protected:
    explicit BCodeEdit(BCodeEditPrivate &d, QWidget *parent = 0);
public:
    void setReadOnly(bool ro);
    void setModification(bool modified);
    void setEditFont(const QFont &fnt);
    void setEditMode(EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setEditAutoIndentationEnabled(bool enabled);
    void setLineNumberWidgetVisible(bool b);
    void setExtraSelections(const ExtraSelectionList &list);
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText);
    int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText);
    bool isReadOnly() const;
    bool isModified() const;
    bool hasSelection() const;
    bool isCutAvailable() const;
    bool isCopyAvailable() const;
    bool isPasteAvailable() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    QFont editFont() const;
    EditMode editMode() const;
    int editLineLength() const;
    BeQt::TabWidth editTabWidth() const;
    bool isEditAutoIndentationEnabled() const;
    bool isLineNumberWidgetVisible() const;
    ExtraSelectionList extraSelections() const;
    int cursorPosition() const;
    QPoint cursorPositionRowColumn() const;
    int cursorPositionForRowColumn(const QPoint &pos) const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    int selectionStart() const;
    int selectionEnd() const;
    QPoint selectionStartRowColumn() const;
    QPoint selectionEndRowColumn() const;
    QMenu *createContextMenu();
    bool isBuisy() const;
public Q_SLOTS:
    void setFocus();
    void activateWindow();
    void setText(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void switchMode();
    void insertText(const QString &txt);
    void clear();
    void moveCursor(int pos);
    void moveCursor(const QPoint &pos);
    void selectText(const QPoint &start, const QPoint &end);
    void selectText(int start, int end);
    void selectLines(int firstLine, int lastLine);
    void selectAll();
    void deselectText();
    void cut();
    void copy();
    void paste();
    void deleteSelection();
    void undo();
    void redo();
protected:
    BPlainTextEdit *innerEdit() const;
    QTextDocument *innerDocument() const;
Q_SIGNALS:
    void textChanged();
    void readOnlyChanged(bool ro);
    void modificationChanged(bool modified);
    void selectionChanged();
    void hasSelectionChanged(bool hasSelection);
    void cutAvailableChanged(bool available);
    void copyAvailableChanged(bool available);
    void pasteAvailableChanged(bool available);
    void undoAvailableChanged(bool available);
    void redoAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void cursorPositionChanged(int pos);
    void cursorPositionChanged(const QPoint &pos);
    void buisyChanged(bool buisy);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
private:
    friend class BAbstractCodeEditorDocument;
    friend class BAbstractCodeEditorDocumentPrivate;
    friend class BCodeEditorDocument;
    friend class BCodeEditorDocumentPrivate;
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H
