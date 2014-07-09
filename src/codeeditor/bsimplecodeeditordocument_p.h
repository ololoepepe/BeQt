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

#ifndef BSIMPLECODEEDITORDOCUMENT_P_H
#define BSIMPLECODEEDITORDOCUMENT_P_H

class BCodeEditor;
class BLineNumberWidget;

class QEvent;
class QKeyEvent;
class QPlainTextEdit;
class QString;
class QTextDocument;
class QWidget;

#include "bsimplecodeeditordocument.h"

#include "babstractcodeeditordocument_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QVariant>

/*============================================================================
================================ BSimpleCodeEditorDocumentPrivate ============
============================================================================*/

class B_CODEEDITOR_EXPORT BSimpleCodeEditorDocumentPrivate : public BAbstractCodeEditorDocumentPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSimpleCodeEditorDocument)
public:
    bool autoIndentation;
    bool blockFilter;
    BLineNumberWidget *lnwgt;
    QPlainTextEdit *ptedt;
    BeQt::TabWidth tabWidth;
public:
    explicit BSimpleCodeEditorDocumentPrivate(BSimpleCodeEditorDocument *q, BCodeEditor *editor);
    ~BSimpleCodeEditorDocumentPrivate();
public:
    static BAbstractCodeEditorDocument::TextProcessingResult procFunc(const QString &text, const QVariant &v);
public:
    QWidget *createEdit(QTextDocument **doc);
    bool eventFilter(QObject *obj, QEvent *e);
    void handleReturn(QKeyEvent *e);
    void handleTab();
    void init();
    bool keyPressEvent(QKeyEvent *e);
public Q_SLOTS:
    void cursorPositionChanged();
    void selectionChanged();
    void updatePasteAvailable(bool available);
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocumentPrivate)
};

#endif // BSIMPLECODEEDITORDOCUMENT_P_H
