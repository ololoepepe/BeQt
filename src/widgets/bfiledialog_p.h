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

class ProxyModel;

class QLayout;
class QLabel;
class QComboBox;
class QEvent;

#include "bfiledialog.h"
#include "btextcodecmenu.h"
#include "btextcodeccombobox.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

class B_WIDGETS_EXPORT BFileDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BFileDialog)
public:
    explicit BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle,
                                const QString &topDir = QString());
    ~BFileDialogPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    bool pathFits(const QString &path) const;
public Q_SLOTS:
    void checkHistory();
    void checkGoToParent();
    void checkLineEdit(const QString &text);
public:
    const BTextCodecMenu::Style CmboxStyle;
    const QString TopDir;
public:
    int maxHistorySize;
    ProxyModel *proxy;
    QLayout *lt;
      QLabel *lblEncodings;
      BTextCodecComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_H
