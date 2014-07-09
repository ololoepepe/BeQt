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

#ifndef BEDITEDITORMODULE_P_H
#define BEDITEDITORMODULE_P_H

class BAbstractCodeEditorDocument;

class QAction;

#include "bediteditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/private/bbase_p.h>

#include <QList>
#include <QObject>

/*============================================================================
================================ BEditEditorModulePrivate ====================
============================================================================*/

class B_CODEEDITOR_EXPORT BEditEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BEditEditorModule)
public:
    QAction *actCopy;
    QAction *actCut;
    QAction *actPaste;
    QAction *actRedo;
    QAction *actSwitchMode;
    QAction *actUndo;
    BAbstractCodeEditorDocument *document;
public:
    explicit BEditEditorModulePrivate(BEditEditorModule *q);
    ~BEditEditorModulePrivate();
public:
    void checkActions();
    void checkSwitchModeAction();
    void init();
    void resetSwitchModeAction(bool bm);
    void setDocument(BAbstractCodeEditorDocument *doc);
public Q_SLOTS:
    void actSwitchModeTriggered();
    void retranslateUi();
private:
    Q_DISABLE_COPY(BEditEditorModulePrivate)
};

#endif // BEDITEDITORMODULE_P_H
