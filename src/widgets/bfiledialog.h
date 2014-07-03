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

#ifndef BFILEDIALOG_H
#define BFILEDIALOG_H

class BFileDialogPrivate;

class QString;
class QTextCodec;
class QByteArray;

#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QFileDialog>
#include <QList>

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

class B_WIDGETS_EXPORT BFileDialog : public QFileDialog, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BFileDialog)
public:
    explicit BFileDialog(QWidget *parent = 0);
    explicit BFileDialog(const QString &topDir, QWidget *parent = 0);
    explicit BFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent = 0);
    explicit BFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent = 0);
    ~BFileDialog();
protected:
    explicit BFileDialog(BFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setMaxHistorySize(int sz);
    void restoreState(const QByteArray &ba);
    bool codecSelectionEnabled() const;
    int maxHistorySize() const;
    QString topDir() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    QByteArray saveState() const;
public Q_SLOTS:
    void setCodecSelectionEnabled(bool b);
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
Q_SIGNALS:
    void codecChanged(QTextCodec *codec);
    void codecNameChanged(const QString &codecName);
private:
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H
