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

class QByteArray;
class QString;
class QTextCodec;

#include "btextcodecmenu.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QFileDialog>
#include <QList>
#include <QObject>

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
    bool codecSelectionEnabled() const;
    bool lineFeedSelectionEnabled() const;
    int maxHistorySize() const;
    void restoreState(const QByteArray &ba);
    QByteArray saveState() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    BeQt::LineFeed selectedLineFeed() const;
    void setMaxHistorySize(int sz);
    QString topDir() const;
public Q_SLOTS:
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
    void selectLineFeed(BeQt::LineFeed lineFeed);
    void setCodecSelectionEnabled(bool b);
    void setLineFeedSelectionEnabled(bool b);
Q_SIGNALS:
    void codecChanged(QTextCodec *codec);
    void codecNameChanged(const QString &codecName);
    void lineFeedChanged(BeQt::LineFeed lineFeed);
private:
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H
