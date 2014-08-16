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

#ifndef BSIMPLECODEEDITORDOCUMENT_H
#define BSIMPLECODEEDITORDOCUMENT_H

class BSimpleCodeEditorDocumentPrivate;

class BCodeEditor;

class QFont;
class QPoint;
class QRegExp;
class QString;
class QWidget;

#include "babstractcodeeditordocument.h"

#include <BeQtCore/BeQt>

#include <QList>
#include <QObject>
#include <QTextDocument>
#include <QVariant>

/*============================================================================
================================ BSimpleCodeEditorDocument ===================
============================================================================*/

class B_CODEEDITOR_EXPORT BSimpleCodeEditorDocument : public BAbstractCodeEditorDocument
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSimpleCodeEditorDocument)
public:
    explicit BSimpleCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BSimpleCodeEditorDocument();
protected:
    explicit BSimpleCodeEditorDocument(BSimpleCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    int cursorPositionForRowColumn(const QPoint &pos) const;
    QFont editFont() const;
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
    void setEditTabWidth(BeQt::TabWidth tw);
    void setLineNumberWidgetVisible(bool b);
    void setModification(bool modified);
    void setReadOnly(bool ro);
    QString text(bool full = false) const;
protected:
    void activateWindowImplementation();
    void clearImplementation();
    void copyImplementation();
    QWidget *createEdit(QTextDocument **doc = 0);
    QPoint cursorPositionRowColumnImplementation() const;
    void cutImplementation();
    void deleteSelectionImplementation();
    void deselectTextImplementation();
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
    TextProcessingFunction textPreprocessingFunction() const;
    QVariant textPreprocessingUserData();
    void undoImplementation();
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
