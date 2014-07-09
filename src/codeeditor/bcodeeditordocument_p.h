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

#ifndef BCODEEDITORDOCUMENT_P_H
#define BCODEEDITORDOCUMENT_P_H

class BCodeEdit;
class BCodeEditor;
class BSplittedLinesDialog;

class QTextDocument;
class QWidget;

#include "bcodeeditordocument.h"

#include "babstractcodeeditordocument_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QObject>

/*============================================================================
================================ BCodeEditorDocumentPrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocumentPrivate : public BAbstractCodeEditorDocumentPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEditorDocument)
public:
    BCodeEdit *cedt;
    BSplittedLinesDialog *sld;
public:
    explicit BCodeEditorDocumentPrivate(BCodeEditorDocument *q, BCodeEditor *editor);
    ~BCodeEditorDocumentPrivate();
public:
    QWidget *createEdit(QTextDocument **doc);
    void init();
private:
    Q_DISABLE_COPY(BCodeEditorDocumentPrivate)
};

#endif // BCODEEDITORDOCUMENT_P_H
