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

#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BSplittedLinesDialog;
class BCodeEditor;

class QWidget;
class QPoint;
class QFont;
class QString;

#include "babstractcodeeditordocument.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QTextDocument>

/*============================================================================
================================ BCodeEditorDocument =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocument : public BAbstractCodeEditorDocument
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEditorDocument)
public:
    explicit BCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BCodeEditorDocument();
protected:
    explicit BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    void setReadOnly(bool ro);
    void setModification(bool modified);
    void setEditFont(const QFont &fnt);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setEditAutoIndentationEnabled(bool enabled);
    void setLineNumberWidgetVisible(bool b);
    void setCurrentLineHighlightingEnabled(bool b);
    void setHighlightedLineColor(const QColor &c);
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText);
    int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText);
    void installInnerEventFilter(QObject *filter);
    void removeInnerEventFilter(QObject *filter);
    QFont editFont() const;
    BeQt::TabWidth editTabWidth() const;
    bool isEditAutoIndentationEnabled() const;
    bool isLineNumberWidgetVisible() const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    int selectionStart() const;
    int selectionEnd() const;
    QPoint selectionStartRowColumn() const;
    QPoint selectionEndRowColumn() const;
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setSplittedLinesDialog(BSplittedLinesDialog *dlg);
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BSplittedLinesDialog *splittedLinesDialog() const;
public Q_SLOTS:
    void switchMode();
protected:
    QWidget *createEdit(QTextDocument **doc = 0);
    void setFocusImplementation();
    void activateWindowImplementation();
    void setTextImplementation(const QString &txt);
    void insertTextImplementation(const QString &txt);
    void clearImplementation();
    void moveCursorImplementation(int pos);
    void moveCursorImplementation(const QPoint &pos);
    void selectTextImplementation(const QPoint &start, const QPoint &end);
    void selectTextImplementation(int start, int end);
    void selectLinesImplementation(int firstLine, int lastLine);
    void selectAllImplementation();
    void deselectTextImplementation();
    void cutImplementation();
    void copyImplementation();
    void pasteImplementation();
    void deleteSelectionImplementation();
    void undoImplementation();
    void redoImplementation();
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    void installDropHandler(QObject *handler);
    QPoint cursorPositionRowColumnImplementation() const;
    int cursorPositionForRowColumn(const QPoint &pos) const;
Q_SIGNALS:
    void editModeChanged(BCodeEdit::EditMode mode);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
