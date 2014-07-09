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

#ifndef BSEARCHEDITORMODULE_H
#define BSEARCHEDITORMODULE_H

class BSearchEditorModulePrivate;

class BAbstractCodeEditorDocument;
class BCodeEditor;

class QAction;
class QByteArray;
class QString;

#include "babstracteditormodule.h"

#include <BeQtCore/BBase>

#include <QList>
#include <QObject>

/*============================================================================
================================ BSearchEditorModule =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSearchEditorModule)
public:
    enum Action
    {
        FindAction = 1,
        FindNextAction
    };
public:
    explicit BSearchEditorModule(QObject *parent = 0);
    ~BSearchEditorModule();
protected:
    explicit BSearchEditorModule(BSearchEditorModulePrivate &d, QObject *parent = 0);
public:
    QAction *action(int type);
    QList<QAction *> actions(bool extended = false);
    bool backwardOrder() const;
    Qt::CaseSensitivity caseSensitivity() const;
    bool cyclicSearch() const;
    QString id() const;
    bool matchWholeWords() const;
    int maximumHistorySize() const;
    bool regexp() const;
    bool replaceEnabled() const;
    void restoreState(const QByteArray &state);
    QByteArray saveState() const;
    void setBackwardOrder(bool enabled);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setCyclicSearch(bool enabled);
    void setMatchWholeWords(bool enabled);
    void setMaximumHistorySize(int sz);
    void setRegexp(bool b);
    void setReplaceEnabled(bool enabled);
public Q_SLOTS:
    void find();
    void findNext(bool showDialog = true);
protected:
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
Q_SIGNALS:
    void message(const QString &msg);
    void textFound(const QString &text);
    void textNotFound(const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
private:
    Q_DISABLE_COPY(BSearchEditorModule)
};

#endif // BSEARCHEDITORMODULE_H
