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

class QWidget;
class QString;
class QByteArray;
class QStringList;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QGroupBox;
class QCheckBox;
class QDialogButtonBox;
class QPushButton;
class QMenu;
class QAction;
class QString;
class QEvent;

#include "bsearcheditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QDialog>
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
    void setRegexp(bool b);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setMatchWholeWords(bool enabled);
    void setBackwardOrder(bool enabled);
    void setCyclicSearch(bool enabled);
    void setSearchHistory(const QStringList &list);
    void setReplaceHistory(const QStringList &list);
    void setMaximumHistorySize(int sz);
    void setReplaceEnabled(bool enabled);
    void setDocument(BAbstractCodeEditorDocument *doc);
    void restoreState(const QByteArray &ba);
    bool regexp() const;
    Qt::CaseSensitivity caseSensitivity() const;
    bool matchWholeWords() const;
    bool backwardOrder() const;
    bool cyclicSearch() const;
    QStringList searchHistory() const;
    QStringList replaceHistory() const;
    int maximumHistorySize() const;
    bool replaceEnabled() const;
    BAbstractCodeEditorDocument *document() const;
    bool findNextAvailable() const;
    bool replaceNextAvailable() const;
    QByteArray saveState() const;
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
    explicit BSearchDialogPrivate(BSearchDialog *q);
    ~BSearchDialogPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    void appendHistory(QComboBox *cmbox);
    void emitTextReplaced(int count, const QString &oldText, const QString &newText);
    QTextDocument::FindFlags createFindFlags() const;
    QString windowTitle() const;
public Q_SLOTS:
    void retranslateUi();
    void checkSearchReplace();
    void actSelectionTriggered();
    void actDocumentTriggered();
public:
    BAbstractCodeEditorDocument *document;
    QVBoxLayout *vlt;
      QHBoxLayout *hltSearch;
        QLabel *lblSearch;
        QComboBox *cmboxSearch;
      QHBoxLayout *hltReplace;
        QLabel *lblReplace;
        QComboBox *cmboxReplace;
      QGroupBox *gboxOptions;
        QVBoxLayout *vltOptions;
          QCheckBox *cboxRegexp;
          QCheckBox *cboxCaseSensitive;
          QCheckBox *cboxWholeWords;
          QCheckBox *cboxBackwardOrder;
          QCheckBox *cboxCyclic;
      QDialogButtonBox *dlgbbox;
        //Close
        QPushButton *btnReplaceAll;
          QMenu *mnuReplaceAll;
            QAction *actSelection;
            QAction *actDocument;
            QAction *actAllDocuments;
        QPushButton *btnReplace;
        QPushButton *btnFind;
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
    explicit BSearchEditorModulePrivate(BSearchEditorModule *q);
    ~BSearchEditorModulePrivate();
public:
    void init();
    void setDialogParent(QWidget *parent = 0);
    QString createNotFoundMessage(const QString &text);
public Q_SLOTS:
    void retranslateUi();
    void textFound(bool found, const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
public:
    BSearchDialog *sdlg;
    QAction *actFind;
    QAction *actFindNext;
private:
    Q_DISABLE_COPY(BSearchEditorModulePrivate)
};

#endif // BSEARCHEDITORMODULE_P_H
