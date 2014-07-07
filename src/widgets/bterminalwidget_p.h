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

#ifndef BTERMINALWIDGET_P_H
#define BTERMINALWIDGET_P_H

class BAbstractTerminalDriver;
class BPlainTextEdit;

class QAction;
class QEvent;
class QKeyEvent;
class QMenu;
class QPoint;

#include "bterminalwidget.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>

/*============================================================================
================================ BTerminalWidgetPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BTerminalWidgetPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTerminalWidget)
public:
    const bool NormalMode;
public:
    QAction *actCopy;
    QAction *actPaste;
    int currentHistory;
    int currentProcessHistory;
    BAbstractTerminalDriver *driver;
    QStringList history;
    bool historyEnabled;
    int lastLineLength;
    QMenu *mnu;
    QStringList processHistory;
    bool processHistoryEnabled;
    BPlainTextEdit *ptedt;
    int terminatingKey;
    int terminatingModifiers;
    QString terminatingSymbols;
public:
    explicit BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode);
    ~BTerminalWidgetPrivate();
public:
    static QTextCharFormat createStandardFormat(BTerminalWidget::StandardFormat format);
public:
    void appendLine(const QString &text = QString(), const QTextCharFormat &format = QTextCharFormat());
    void appendText(const QString &text, const QTextCharFormat &format = QTextCharFormat());
    QString constructErrorString(const QString &error) const;
    bool eventFilter(QObject *o, QEvent *e);
    bool handleKeyPress(QKeyEvent *e);
    void handleReturn();
    void handleUpDown(bool up);
    void init();
    bool pasteAvailable() const;
    void scrollDown();
    void setDriver(BAbstractTerminalDriver *drv);
public Q_SLOTS:
    void actCopyTriggered();
    void actPasteTriggered();
    void blockTerminal();
    void customContextMenuRequested(const QPoint &pos);
    void finished(int exitCode);
    void read();
    void retranslateUi();
    void unblockTerminal();
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivate)
};

#endif // BTERMINALWIDGET_P_H
