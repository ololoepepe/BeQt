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
class BCodeEditor;
class BAbstractCodeEditorDocument;

class QString;
class QAction;
class QByteArray;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QList>

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
        FindAction,
        FindNextAction
    };
public:
    explicit BSearchEditorModule(QObject *parent = 0);
    ~BSearchEditorModule();
protected:
    explicit BSearchEditorModule(BSearchEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QAction *action(int type);
    QList<QAction *> actions(bool extended = false);
    QByteArray saveState() const;
    void restoreState(const QByteArray &state);
    void setRegexp(bool b);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setMatchWholeWords(bool enabled);
    void setBackwardOrder(bool enabled);
    void setCyclicSearch(bool enabled);
    void setMaximumHistorySize(int sz);
    void setReplaceEnabled(bool enabled);
    bool regexp() const;
    Qt::CaseSensitivity caseSensitivity() const;
    bool matchWholeWords() const;
    bool backwardOrder() const;
    bool cyclicSearch() const;
    int maximumHistorySize() const;
    bool replaceEnabled() const;
public Q_SLOTS:
    void find();
    void findNext(bool showDialog = true);
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
Q_SIGNALS:
    void textFound(const QString &text);
    void textNotFound(const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
    void message(const QString &msg);
private:
    Q_DISABLE_COPY(BSearchEditorModule)
};

#endif // BSEARCHEDITORMODULE_H
