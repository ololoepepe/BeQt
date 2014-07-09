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

#ifndef BINDICATORSEDITORMODULE_P_H
#define BINDICATORSEDITORMODULE_P_H

class QComboBox;
class QLabel;
class QString;

#include "bindicatorseditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>
#include <QList>

/*============================================================================
================================ BIndicatorsEditorModulePrivate ==============
============================================================================*/

class B_CODEEDITOR_EXPORT BIndicatorsEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BIndicatorsEditorModule)
public:
    struct FileTypeInfo
    {
        QString id;
        QString name;
    };
public:
    QComboBox *cmboxFileType;
    QLabel *lblCursorPos;
    QLabel *lblEncoding;
public:
    explicit BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q);
    ~BIndicatorsEditorModulePrivate();
public:
    void init();
    void updateCursorPosIndicator();
    void updateEncodingIndicator();
    void updateFileTypeIndicator();
public Q_SLOTS:
    void cmboxFileTypeCurrentIndexChanged(int index);
    void retranslateUi();
private:
    Q_DISABLE_COPY(BIndicatorsEditorModulePrivate)
};

#endif // BINDICATORSEDITORMODULE_P_H
