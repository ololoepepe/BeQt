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

#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BPlainTextEditExtendedPrivate;

class BLineNumberWidget;

class QBrush;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class QStringList;
class QTextBlock;

#include "bcodeedit.h"

#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtWidgets/BPlainTextEdit>
#include <BeQtWidgets/private/bplaintextedit_p.h>

#include <QAbstractTextDocumentLayout>
#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QRectF>
#include <QString>
#include <QVector>

/*============================================================================
================================ BPlainTextEditExtended ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BPlainTextEditExtended : public BPlainTextEdit
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPlainTextEditExtended)
public:
    enum SelectionMode
    {
        BlockSelection = 1,
        NormalSelection
    };
public:
    struct SelectionRange
    {
        int end;
        int start;
    public:
        SelectionRange()
        {
            end = -1;
            start = -1;
        }
    };
public:
    explicit BPlainTextEditExtended(QWidget *parent = 0);
    ~BPlainTextEditExtended();
protected:
    explicit BPlainTextEditExtended(BPlainTextEditExtendedPrivate &d, QWidget *parent = 0);
public:
    bool blockMode() const;
    QVector<SelectionRange> selectionRanges() const;
    void setBlockMode(bool enabled);
    void updateSelection();
protected:
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEditExtended)
};

/*============================================================================
================================ BPlainTextEditExtendedPrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BPlainTextEditExtendedPrivate : public BPlainTextEditPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPlainTextEditExtended)
public:
    bool blockMode;
    bool hasSelection;
    QVector<BPlainTextEditExtended::SelectionRange> selectionRanges;
public:
    explicit BPlainTextEditExtendedPrivate(BPlainTextEditExtended *q);
    ~BPlainTextEditExtendedPrivate();
public:
    static inline void fillBackground(QPainter *painter, const QRectF &rect, QBrush brush,
                                      QRectF gradientRect = QRectF());
public:
    void emulateShiftPress();
    inline QAbstractTextDocumentLayout::PaintContext getPaintContext() const;
    void init();
public Q_SLOTS:
    void selectionChanged();
private:
    Q_DISABLE_COPY(BPlainTextEditExtendedPrivate)
};

/*============================================================================
================================ BCodeEditPrivate ============================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEdit)
public:
    struct ProcessTextResult
    {
        QString newText;
        QList<BCodeEdit::SplittedLinesRange> splittedLinesRanges;
    };
public:
    typedef QFuture<ProcessTextResult> ProcessTextResultFuture;
    typedef QFutureWatcher<ProcessTextResult> ProcessTextResultFutureWatcher;
public:
    bool autoIndentation;
    bool blockMode;
    bool buisy;
    bool copyAvailable;
    int cursorPosition;
    QPoint cursorPositionRowColumn;
    bool hasSelection;
    int lineLength;
    BLineNumberWidget *lnwgt;
    bool onceEdited;
    bool pasteAvailable;
    BPlainTextEditExtended *ptedt;
    bool redoAvailable;
    BeQt::TabWidth tabWidth;
    int tmpLineLength;
    bool undoAvailable;
public:
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
public:
    static BCodeEdit::SplittedLinesRange createSplittedLinesRange();
    static QString makeBlock(const QString &text, int *length = 0);
    static void makeBlock(QString *text, int *length = 0);
    static QStringList processLine(const QString &line, int ll, BeQt::TabWidth tw);
    static ProcessTextResult processText(const QString &text, int ll, BeQt::TabWidth tw);
    static bool testBlock(const QString &text, int *length = 0);
    static bool testBlock(const QStringList &lines, int *length = 0);
public:
    void blockHighlighter(bool block);
    BCodeEdit::SplittedLinesRange deleteSelection();
    void emitLineSplitted(const BCodeEdit::SplittedLinesRange &range);
    void emitLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> &ranges);
    bool eventFilter(QObject *obj, QEvent *e);
    void handleBackspace();
    void handleCtrlBackspace();
    void handleCtrlDelete();
    void handleCtrlLeft();
    void handleCtrlRight();
    void handleDelete();
    void handleEnd(bool ctrl = false);
    void handleLeft(bool shift = false);
    void handleReturn();
    void handleShiftEnd();
    void handleSpace();
    void handleTab();
    void init();
    void insertText(const QString &txt, bool asKeyPress = false);
    inline bool keyPressEvent(QKeyEvent *e);
    inline bool mouseDoubleClickEvent(QMouseEvent *e);
    inline bool mousePressEvent(QMouseEvent *e);
    void move(int key);
    void requestRehighlightBlock(const QTextBlock &block);
    void seletAll();
    void setBuisy(bool b);
    void setText(const QString &txt, int asyncIfLongerThan);
public Q_SLOTS:
    void delayedSetLineLength();
    void emitModificationChanged(bool modified);
    void emitSelectionChanged();
    void emitTextChanged();
    void parceTaskFinished();
    void setTextToEmptyLine();
    void updateCopyAvailable(bool available);
    void updateCursorPosition();
    void updateHasSelection();
    void updatePasteAvailable(bool available);
    void updateRedoAvailable(bool available);
    void updateUndoAvailable(bool available);
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
};

#endif // BCODEEDIT_P_H
