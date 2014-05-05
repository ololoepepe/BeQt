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

#ifndef BABSTRACTDOCUMENTDRIVER_P_H
#define BABSTRACTDOCUMENTDRIVER_P_H

class BCodeEditor;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BAbstractDocumentDriverPrivate ==============
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractDocumentDriverPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractDocumentDriver)
public:
    explicit BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q);
    ~BAbstractDocumentDriverPrivate();
public:
    void init();
    void setEditor(BCodeEditor *edr);
public:
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractDocumentDriverPrivate)
};

#endif // BABSTRACTDOCUMENTDRIVER_P_H
