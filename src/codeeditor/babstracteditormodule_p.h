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

#ifndef BABSTRACTEDITORMODULE_P_H
#define BABSTRACTEDITORMODULE_P_H

class BCodeEditor;
class BCodeEditorPrivate;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>

/*============================================================================
================================ BAbstractEditorModulePrivate ================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractEditorModulePrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractEditorModule)
public:
    explicit BAbstractEditorModulePrivate(BAbstractEditorModule *q);
    ~BAbstractEditorModulePrivate();
public:
    void init();
    void setEditor(BCodeEditor *edr);
public:
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractEditorModulePrivate)
    friend class BCodeEditorPrivate;
};

#endif // BABSTRACTEDITORMODULE_P_H
