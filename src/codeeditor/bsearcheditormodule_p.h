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

#ifndef BSEARCHEDITORMODULE_P_H
#define BSEARCHEDITORMODULE_P_H

class BSearchDialogPrivate;

class BAbstractCodeEditorDocument;

class QAction;
class QByteArray;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QEvent;
class QGroupBox;
class QLabel;
class QMenu;
class QPushButton;
class QString;
class QStringList;
class QWidget;

#include "bsearcheditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QDialog>
#include <QObject>
#include <QTextDocument>

/*============================================================================
================================ BSearchDialog ===============================
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSearchDialog)
public:
    explicit BSearchDialog(QWidget *parent = 0);
    explicit BSearchDialog(bool replaceEnabled, QWidget *parent = 0);
    ~BSearchDialog();
protected:
    explicit BSearchDialog(BSearchDialogPrivate &d, QWidget *parent = 0);
public:
    bool backwardOrder() const;
    Qt::CaseSensitivity caseSensitivity() const;
    bool cyclicSearch() const;
    BAbstractCodeEditorDocument *document() const;
    bool findNextAvailable() const;
    bool matchWholeWords() const;
    int maximumHistorySize() const;
    bool regexp() const;
    bool replaceEnabled() const;
    QStringList replaceHistory() const;
    bool replaceNextAvailable() const;
    void restoreState(const QByteArray &ba);
    QByteArray saveState() const;
    QStringList searchHistory() const;
    void setBackwardOrder(bool enabled);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setCyclicSearch(bool enabled);
    void setDocument(BAbstractCodeEditorDocument *doc);
    void setMatchWholeWords(bool enabled);
    void setMaximumHistorySize(int sz);
    void setRegexp(bool b);
    void setReplaceEnabled(bool enabled);
    void setReplaceHistory(const QStringList &list);
    void setSearchHistory(const QStringList &list);
public Q_SLOTS:
    void findNext();
    void replaceNext();
Q_SIGNALS:
    void findNextAvailableChanged(bool available);
    void replaceNextAvailableChanged(bool available);
    void textFound(bool found, const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
private:
    Q_DISABLE_COPY(BSearchDialog)
};

/*============================================================================
================================ BSearchDialogPrivate ========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSearchDialog)
public:
    static const int DefMaximumHistorySize;
public:
    QAction *actAllDocuments;
    QAction *actDocument;
    QAction *actSelection;
    QPushButton *btnFind;
    QPushButton *btnReplace;
    QPushButton *btnReplaceAll;
    QCheckBox *cboxBackwardOrder;
    QCheckBox *cboxCaseSensitive;
    QCheckBox *cboxCyclic;
    QCheckBox *cboxRegexp;
    QCheckBox *cboxWholeWords;
    QComboBox *cmboxReplace;
    QComboBox *cmboxSearch;
    QDialogButtonBox *dlgbbox;
    BAbstractCodeEditorDocument *document;
    QGroupBox *gboxOptions;
    QLabel *lblReplace;
    QLabel *lblSearch;
    QMenu *mnuReplaceAll;
public:
    explicit BSearchDialogPrivate(BSearchDialog *q);
    ~BSearchDialogPrivate();
public:
    void appendHistory(QComboBox *cmbox);
    QTextDocument::FindFlags createFindFlags() const;
    void emitTextReplaced(int count, const QString &oldText, const QString &newText);
    bool eventFilter(QObject *o, QEvent *e);
    void init();
    QString windowTitle() const;
public Q_SLOTS:
    void actDocumentTriggered();
    void actSelectionTriggered();
    void checkSearchReplace();
    void retranslateUi();
private:
    Q_DISABLE_COPY(BSearchDialogPrivate)
};

/*============================================================================
================================ BSearchEditorModulePrivate ==================
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSearchEditorModule)
public:
    QAction *actFind;
    QAction *actFindNext;
    BSearchDialog *sdlg;
public:
    explicit BSearchEditorModulePrivate(BSearchEditorModule *q);
    ~BSearchEditorModulePrivate();
public:
    QString createNotFoundMessage(const QString &text);
    void init();
    void setDialogParent(QWidget *parent = 0);
public Q_SLOTS:
    void retranslateUi();
    void textFound(bool found, const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
private:
    Q_DISABLE_COPY(BSearchEditorModulePrivate)
};

#endif // BSEARCHEDITORMODULE_P_H
