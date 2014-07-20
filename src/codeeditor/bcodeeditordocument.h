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

class BCodeEditor;
class BSplittedLinesDialog;

class QFont;
class QPoint;
class QString;
class QWidget;

#include "babstractcodeeditordocument.h"

#include "bcodeedit.h"

#include <BeQtCore/BeQt>

#include <QList>
#include <QObject>
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
    int cursorPositionForRowColumn(const QPoint &pos) const;
    QFont editFont() const;
    int editLineLength() const;
    BCodeEdit::EditMode editMode() const;
    BeQt::TabWidth editTabWidth() const;
    void installInnerEventFilter(QObject *filter);
    void installInnerViewportEventFilter(QObject *filter);
    bool isEditAutoIndentationEnabled() const;
    bool isLineNumberWidgetVisible() const;
    void removeInnerEventFilter(QObject *filter);
    int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText);
    bool replaceNext(const QString &newText);
    QString selectedText(bool full = false) const;
    int selectionEnd() const;
    QPoint selectionEndRowColumn() const;
    int selectionStart() const;
    QPoint selectionStartRowColumn() const;
    void setEditAutoIndentationEnabled(bool enabled);
    void setEditFont(const QFont &fnt);
    void setEditLineLength(int ll);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setLineNumberWidgetVisible(bool b);
    void setModification(bool modified);
    void setReadOnly(bool ro);
    void setSplittedLinesDialog(BSplittedLinesDialog *dlg);
    BSplittedLinesDialog *splittedLinesDialog() const;
    QString text(bool full = false) const;
public Q_SLOTS:
    void switchMode();
protected:
    void activateWindowImplementation();
    void clearImplementation();
    void copyImplementation();
    QWidget *createEdit(QTextDocument **doc = 0);
    QPoint cursorPositionRowColumnImplementation() const;
    void cutImplementation();
    void deleteSelectionImplementation();
    void deselectTextImplementation();
    ExtraSelectionList extraSelectionsImplementation() const;
    bool findNextImplementation(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexpImplementation(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    void insertTextImplementation(const QString &txt);
    void moveCursorImplementation(int pos);
    void moveCursorImplementation(const QPoint &pos);
    void pasteImplementation();
    void redoImplementation();
    void selectAllImplementation();
    void selectLinesImplementation(int firstLine, int lastLine);
    void selectTextImplementation(const QPoint &start, const QPoint &end);
    void selectTextImplementation(int start, int end);
    void setExtraSelectionsImplementation(const ExtraSelectionList &list);
    void setFocusImplementation();
    void setTextImplementation(const QString &txt);
    void undoImplementation();
Q_SIGNALS:
    void editModeChanged(BCodeEdit::EditMode mode);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
