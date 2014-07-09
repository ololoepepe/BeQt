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

#include "bfiledialog.h"
#include "bfiledialog_p.h"

#include "btextcodeccombobox.h"
#include "btextcodecmenu.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QByteArray>
#include <QCompleter>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTimer>
#include <QToolButton>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BProxyModel =================================
============================================================================*/

/*============================== Public constructors =======================*/

BProxyModel::BProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    prevent = false;
}

/*============================== Public methods ============================*/

void BProxyModel::setPreventAll(bool b)
{
    if (b == prevent)
        return;
    prevent = b;
    invalidateFilter();
}

/*============================== Protected methods =========================*/

bool BProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    return !prevent && QSortFilterProxyModel::filterAcceptsRow(row, parent);
}

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle, const QString &topDir) :
    BBaseObjectPrivate(q), CmboxStyle(comboBoxStyle), TopDir(chooseDir(topDir))
{
    //
}

BFileDialogPrivate::~BFileDialogPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QString BFileDialogPrivate::chooseDir(const QString &path)
{
    if (path.isEmpty())
        return "";
    QFileInfo fi(path);
    return fi.isDir() ? QDir::cleanPath(fi.absoluteFilePath()) : QString();
}

/*============================== Public methods ============================*/

bool BFileDialogPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return false;
    int key = static_cast<QKeyEvent *>(e)->key();
    if (o->objectName() == "listView" || o->objectName() == "treeView")
        return (Qt::Key_Backspace == key && !pathFits(q_func()->directory().absolutePath()));
    if (Qt::Key_Return != key && Qt::Key_Enter != key)
        return false;
    QString text = q_func()->findChild<QLineEdit *>("fileNameEdit")->text();
    QString path = QDir::cleanPath(q_func()->directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
    bool a = QDir(text).isAbsolute();
    return !((!a && pathFits(path)) || (a && pathFits(text)));
}

void BFileDialogPrivate::init()
{
    maxHistorySize = 20;
    B_Q(BFileDialog);
    if (!TopDir.isEmpty()) {
        connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(checkGoToParent()));
        connect(q->findChild<QToolButton *>("backButton"), SIGNAL(clicked()), this, SLOT(checkGoToParent()));
        connect(q->findChild<QToolButton *>("forwardButton"), SIGNAL(clicked()), this, SLOT(checkGoToParent()));
        connect(q->findChild<QLineEdit *>("fileNameEdit"), SIGNAL(textChanged(QString)),
                this, SLOT(checkLineEdit(QString)));
        q->findChild<QLineEdit *>("fileNameEdit")->installEventFilter(this);
        q->findChild<QWidget *>("listView")->installEventFilter(this);
        q->findChild<QWidget *>("treeView")->installEventFilter(this);
        QAbstractItemView *view = q->findChild<QLineEdit *>("fileNameEdit")->completer()->popup();
        view->installEventFilter(this);
        QAbstractItemModel *mdl = view->model();
        proxy = new BProxyModel(this);
        view->setModel(proxy);
        proxy->setSourceModel(mdl);
        q->findChild<QWidget *>("lookInCombo")->setEnabled(false);
        q->findChild<QWidget *>("sidebar")->setEnabled(false);
        checkGoToParent();
    } else {
        proxy = 0;
    }
    connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(checkHistory()));
    q->setOption(BFileDialog::DontUseNativeDialog, true);
    QLayout *lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(lblEncodings);
      cmboxEncodings = new BTextCodecComboBox(CmboxStyle);
      connect(cmboxEncodings, SIGNAL(codecChanged(QTextCodec *)), q, SIGNAL(codecChanged(QTextCodec *)));
      connect(cmboxEncodings, SIGNAL(codecNameChanged(QString)), q, SIGNAL(codecNameChanged(QString)));
      lt->addWidget(cmboxEncodings);
}

bool BFileDialogPrivate::pathFits(const QString &path) const
{
    return (path.startsWith(TopDir) && path.length() > TopDir.length());
}

/*============================== Public slots ==============================*/

void BFileDialogPrivate::checkHistory()
{
    QStringList list = q_func()->history();
    while (list.size() > maxHistorySize)
        list.removeFirst();
    q_func()->setHistory(list);
}

void BFileDialogPrivate::checkGoToParent()
{
    B_Q(BFileDialog);
    q->findChild<QToolButton *>("toParentButton")->setEnabled(pathFits(q->directory().absolutePath()));
}

void BFileDialogPrivate::checkLineEdit(const QString &text)
{
    QAbstractButton *btn = q_func()->findChild<QDialogButtonBox *>("buttonBox")->buttons().first();
    QString path = QDir::cleanPath(q_func()->directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
    bool a = QDir(text).isAbsolute();
    bool b = ((!a && pathFits(path)) || (a && pathFits(text)));
    proxy->setPreventAll(!b);
    btn->setEnabled(btn->isEnabled() && b);
    if (!b)
        QTimer::singleShot(0, q_func()->findChild<QLineEdit *>("fileNameEdit")->completer()->popup(), SLOT(close()));

}

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialog::BFileDialog(QWidget *parent) :
    QFileDialog(parent), BBaseObject(*new BFileDialogPrivate(this, BTextCodecMenu::StructuredStyle))
{
    d_func()->init();
}

BFileDialog::BFileDialog(const QString &topDir, QWidget *parent) :
    QFileDialog(parent, "", BFileDialogPrivate::chooseDir(topDir)),
    BBaseObject(*new BFileDialogPrivate(this, BTextCodecMenu::StructuredStyle, topDir))
{
    d_func()->init();
}

BFileDialog::BFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent) :
    QFileDialog(parent), BBaseObject(*new BFileDialogPrivate(this, comboBoxStyle))
{
    d_func()->init();
}

BFileDialog::BFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent) :
    QFileDialog(parent, "", BFileDialogPrivate::chooseDir(topDir)),
    BBaseObject(*new BFileDialogPrivate(this, comboBoxStyle, topDir))
{
    d_func()->init();
}

BFileDialog::~BFileDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BFileDialog::BFileDialog(BFileDialogPrivate &d, QWidget *parent) :
    QFileDialog(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BFileDialog::codecSelectionEnabled() const
{
    return d_func()->cmboxEncodings->isVisible();
}

int BFileDialog::maxHistorySize() const
{
    return d_func()->maxHistorySize;
}

void BFileDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    QFileDialog::restoreState(m.value("q_file_dialog_state").toByteArray());
    d_func()->checkHistory();
    selectCodec(m.value("codec_name").toString());
}

QByteArray BFileDialog::saveState() const
{
    QVariantMap m;
    m.insert("q_file_dialog_state", QFileDialog::saveState());
    m.insert("codec_name", selectedCodecName());
    return BeQt::serialize(m);
}

QTextCodec *BFileDialog::selectedCodec() const
{
    return d_func()->cmboxEncodings->selectedCodec();
}

QString BFileDialog::selectedCodecName() const
{
    return d_func()->cmboxEncodings->selectedCodecName();
}

void BFileDialog::setMaxHistorySize(int sz)
{
    if (sz < 1)
        return;
    d_func()->maxHistorySize = sz;
    d_func()->checkHistory();
}

QString BFileDialog::topDir() const
{
    return d_func()->TopDir;
}

/*============================== Public slots ==============================*/

void BFileDialog::selectCodec(QTextCodec *codec)
{
    d_func()->cmboxEncodings->selectCodec(codec);
}

void BFileDialog::selectCodec(const QString &codecName)
{
    d_func()->cmboxEncodings->selectCodec(codecName);
}

void BFileDialog::setCodecSelectionEnabled(bool b)
{
    d_func()->cmboxEncodings->setVisible(b);
}
