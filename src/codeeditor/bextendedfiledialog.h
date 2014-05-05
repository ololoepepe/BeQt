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

#ifndef BEXTENDEDFILEDIALOG_H
#define BEXTENDEDFILEDIALOG_H

class BExtendedFileDialogPrivate;
class BAbstractFileType;

class QString;
class QByteArray;

#include <BeQtCore/BeQtGlobal>
#include <BeQtWidgets/BTextCodecMenu>
#include <BeQtWidgets/BFileDialog>

#include <QObject>
#include <QList>

/*============================================================================
================================ BExtendedFileDialog =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialog : public BFileDialog
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BExtendedFileDialog)
public:
    explicit BExtendedFileDialog(QWidget *parent = 0);
    explicit BExtendedFileDialog(const QString &topDir, QWidget *parent = 0);
    explicit BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent = 0);
    explicit BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent = 0);
    ~BExtendedFileDialog();
protected:
    explicit BExtendedFileDialog(BExtendedFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void selectFileType(BAbstractFileType *ft);
    void selectFileType(const QString &id);
    void restoreState(const QByteArray &ba);
    BAbstractFileType *selectedFileType() const;
    QString selectedFileTypeId() const;
    QByteArray saveState() const;
private:
    Q_DISABLE_COPY(BExtendedFileDialog)
};

#endif // BEXTENDEDFILEDIALOG_H
