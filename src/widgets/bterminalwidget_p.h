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

class BTerminalWidgetPrivate;
class BAbstractTerminalDriver;
class BPlainTextEdit;

class QEvent;
class QString;
class QVBoxLayout;

#include "bterminalwidget.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QTextCharFormat>

/*============================================================================
================================ BTerminalWidgetPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BTerminalWidgetPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTerminalWidget)
public:
    explicit BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode);
    ~BTerminalWidgetPrivate();
public:
    static QTextCharFormat createStandardFormat(BTerminalWidget::StandardFormat format);
public:
    void init();
    bool eventFilter(QObject *object, QEvent *event);
    void setDriver(BAbstractTerminalDriver *drv);
    bool handleKeyPress(int key, int modifiers);
    void scrollDown();
    void appendText( const QString &text, const QTextCharFormat &format = QTextCharFormat() );
    void appendLine( const QString &text = QString(), const QTextCharFormat &format = QTextCharFormat() );
    QString constructErrorString(const QString &error) const;
public Q_SLOTS:
    void read();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
public:
    const bool NormalMode;
public:
    BAbstractTerminalDriver *driver;
    int terminatingKey;
    int terminatingModifiers;
    QString terminatingSymbols;
    int len;
    QVBoxLayout *vlt;
      BPlainTextEdit *ptedt;
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivate)
};

#endif // BTERMINALWIDGET_P_H
