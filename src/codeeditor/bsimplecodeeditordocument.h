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

class QWidget;
class QPoint;
class QFont;
class QString;
class QRegExp;

#include "babstractcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QTextDocument>
#include <QVariantMap>

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
    void setReadOnly(bool ro);
    void setModification(bool modified);
    void setEditFont(const QFont &fnt);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setEditAutoIndentationEnabled(bool enabled);
    void setLineNumberWidgetVisible(bool b);
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool findNextRegexp(const QRegExp &rx, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInSelectionRegexp(const QRegExp &rx, const QString &newText);
    int replaceInDocument(const QString &txt, const QString &newText, QTextDocument::FindFlags flags = 0);
    int replaceInDocumentRegexp(const QRegExp &rx, const QString &newText);
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
    void installDropHandler(QObject *handler);
    void installInnerEventFilter(QObject *filter);
    QPoint cursorPositionRowColumnImplementation() const;
    int cursorPositionForRowColumn(const QPoint &pos) const;
    //TextProcessingFunction textPreprocessingFunction() const;
    //QVariantMap preprocessingUserData();
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
