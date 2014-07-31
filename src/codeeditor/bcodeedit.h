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

class BAbstractCodeEditorDocumentPrivate;
class BCodeEditorDocument;
class BCodeEditorDocumentPrivate;
class BPlainTextEdit;

class QRegExp;

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QFont>
#include <QList>
#include <QPoint>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <QWidget>

/*============================================================================
================================ BCodeEdit ===================================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEdit : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(bool buisy READ isBuisy NOTIFY buisyChanged STORED false)
    Q_PROPERTY(bool copyAvailable READ isCopyAvailable NOTIFY copyAvailableChanged STORED false)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE moveCursor NOTIFY cursorPositionChanged)
    Q_PROPERTY(QPoint cursorPositionRowColumn READ cursorPositionRowColumn WRITE moveCursor
               NOTIFY cursorPositionChanged)
    Q_PROPERTY(bool cutAvailable READ isCutAvailable NOTIFY cutAvailableChanged STORED false)
    Q_PROPERTY(bool editAutoIndentationEnabled READ isEditAutoIndentationEnabled WRITE setEditAutoIndentationEnabled)
    Q_PROPERTY(QFont editFont READ editFont WRITE setEditFont)
    Q_PROPERTY(int editLineLength READ editLineLength WRITE setEditLineLength)
    Q_PROPERTY(EditMode editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged STORED false)
    Q_PROPERTY(QTextDocument * innerDocument READ innerDocument CONSTANT)
    Q_PROPERTY(BPlainTextEdit * innerEdit READ innerEdit CONSTANT)
    Q_PROPERTY(bool lineNumberWidgetVisible READ isLineNumberWidgetVisible WRITE setLineNumberWidgetVisible)
    Q_PROPERTY(bool modified READ isModified NOTIFY modificationChanged STORED false)
    Q_PROPERTY(bool pasteAvailable READ isPasteAvailable NOTIFY pasteAvailableChanged STORED false)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool redoAvailable READ isRedoAvailable NOTIFY redoAvailableChanged STORED false)
    Q_PROPERTY(int selectionEnd READ selectionEnd STORED false)
    Q_PROPERTY(QPoint selectionEndRowColumn READ selectionEndRowColumn STORED false)
    Q_PROPERTY(int selectionStart READ selectionStart STORED false)
    Q_PROPERTY(QPoint selectionStartRowColumn READ selectionStartRowColumn STORED false)
    Q_PROPERTY(bool undoAvailable READ isUndoAvailable NOTIFY undoAvailableChanged STORED false)
    Q_ENUMS(EditMode)
    B_DECLARE_PRIVATE(BCodeEdit)
public:
    enum EditMode
    {
        BlockMode = 1,
        NormalMode
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
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    QMenu *createContextMenu();
    int cursorPosition() const;
    int cursorPositionForRowColumn(const QPoint &pos) const;
    QPoint cursorPositionRowColumn() const;
    QFont editFont() const;
    int editLineLength() const;
    EditMode editMode() const;
    BeQt::TabWidth editTabWidth() const;
    ExtraSelectionList extraSelections() const;
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool hasSelection() const;
    QTextDocument *innerDocument() const;
    BPlainTextEdit *innerEdit() const;
    bool isBuisy() const;
    bool isCopyAvailable() const;
    bool isCutAvailable() const;
    bool isEditAutoIndentationEnabled() const;
    bool isLineNumberWidgetVisible() const;
    bool isModified() const;
    bool isPasteAvailable() const;
    bool isReadOnly() const;
    bool isRedoAvailable() const;
    bool isUndoAvailable() const;
    int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText);
    int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText);
    bool replaceNext(const QString &newText);
    QString selectedText(bool full = false) const;
    int selectionEnd() const;
    QPoint selectionEndRowColumn() const;
    int selectionStart() const;
    QPoint selectionStartRowColumn() const;
    void setEditAutoIndentationEnabled(bool enabled);
    void setEditFont(const QFont &fnt);
    void setEditLineLength(int ll);
    void setEditMode(EditMode mode);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setExtraSelections(const ExtraSelectionList &list);
    void setLineNumberWidgetVisible(bool b);
    void setModification(bool modified);
    void setReadOnly(bool ro);
    QString text(bool full = false) const;
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
    void selectAll();
    void selectLines(int firstLine, int lastLine);
    void selectText(const QPoint &start, const QPoint &end);
    void selectText(int start, int end);
    void setFocus();
    void setText(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void switchMode();
    void undo();
Q_SIGNALS:
    void buisyChanged(bool buisy);
    void copyAvailableChanged(bool available);
    void cursorPositionChanged(int pos);
    void cursorPositionChanged(const QPoint &pos);
    void cutAvailableChanged(bool available);
    void editModeChanged(BCodeEdit::EditMode mode);
    void hasSelectionChanged(bool hasSelection);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
    void modificationChanged(bool modified);
    void pasteAvailableChanged(bool available);
    void readOnlyChanged(bool ro);
    void redoAvailableChanged(bool available);
    void selectionChanged();
    void textChanged();
    void undoAvailableChanged(bool available);
private:
    friend class BAbstractCodeEditorDocument;
    friend class BAbstractCodeEditorDocumentPrivate;
    friend class BCodeEditorDocument;
    friend class BCodeEditorDocumentPrivate;
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H
