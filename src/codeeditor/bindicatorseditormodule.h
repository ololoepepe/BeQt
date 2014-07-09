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

#ifndef BINDICATORSEDITORMODULE_H
#define BINDICATORSEDITORMODULE_H

class BIndicatorsEditorModulePrivate;

class BAbstractCodeEditorDocument;
class BAbstractFileType;
class BCodeEditor;

class QString;
class QWidget;

#include "babstracteditormodule.h"

#include <BeQtCore/BBase>

#include <QList>
#include <QObject>

/*============================================================================
================================ BIndicatorsEditorModule =====================
============================================================================*/

class B_CODEEDITOR_EXPORT BIndicatorsEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BIndicatorsEditorModule)
public:
    enum Indicator
    {
        CursorPositionIndicator = 1,
        EncodingIndicator,
        FileTypeIndicator
    };
public:
    explicit BIndicatorsEditorModule(QObject *parent = 0);
    ~BIndicatorsEditorModule();
protected:
    explicit BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QWidget *widget(int type);
    QList<QWidget *> widgets(bool extended = false);
protected:
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void defaultCodecChanged(const QString &codecName);
    void documentCodecChanged(const QString &codecName);
    void documentCursorPositionChanged(const QPoint &pos);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void fileTypesChanged();
private:
    Q_DISABLE_COPY(BIndicatorsEditorModule)
};

#endif // BINDICATORSEDITORMODULE_H
