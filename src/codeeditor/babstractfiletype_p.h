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

#ifndef BABSTRACTFILETYPE_P_H
#define BABSTRACTFILETYPE_P_H

class BSyntaxHighlighter;

#include "babstractfiletype.h"

#include <BeQtCore/private/bbase_p.h>

/*============================================================================
================================ BAbstractFileTypePrivate ====================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileTypePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractFileType)
public:
    explicit BAbstractFileTypePrivate(BAbstractFileType *q);
    ~BAbstractFileTypePrivate();
public:
    void init();
public:
    BSyntaxHighlighter *highlighter;
private:
    Q_DISABLE_COPY(BAbstractFileTypePrivate)
};

#endif // BABSTRACTFILETYPE_P_H
