/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#ifndef BFILEDIALOG_P_H
#define BFILEDIALOG_P_H

class BTextCodecComboBox;

class QEvent;
class QLabel;
class QModelIndex;

#include "bfiledialog.h"

#include "btextcodecmenu.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>

/*============================================================================
================================ BProxyModel =================================
============================================================================*/

class BProxyModel : public QSortFilterProxyModel
{
private:
    bool prevent;
public:
    explicit BProxyModel(QObject *parent = 0);
public:
    void setPreventAll(bool b);
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

class B_WIDGETS_EXPORT BFileDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BFileDialog)
public:
    const BTextCodecMenu::Style CmboxStyle;
    const QString TopDir;
public:
    BTextCodecComboBox *cmboxEncodings;
    QLabel *lblEncodings;
    int maxHistorySize;
    BProxyModel *proxy;
public:
    explicit BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle,
                                const QString &topDir = QString());
    ~BFileDialogPrivate();
public:
    static QString chooseDir(const QString &path);
public:
    bool eventFilter(QObject *o, QEvent *e);
    void init();
    bool pathFits(const QString &path) const;
public Q_SLOTS:
    void checkGoToParent();
    void checkHistory();
    void checkLineEdit(const QString &text);
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_H
