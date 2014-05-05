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
#include "btextcodecmenu.h"
#include "btextcodeccombobox.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QStringList>
#include <QFileDialog>
#include <QList>
#include <QLayout>
#include <QLabel>
#include <QVariant>
#include <QByteArray>
#include <QVariantMap>
#include <QToolButton>
#include <QDir>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QAbstractButton>
#include <QCompleter>
#include <QAbstractItemView>
#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QCompleter>
#include <QAbstractItemModel>
#include <QTimer>

#include <QDebug>

/*============================================================================
================================ ProxyModel ==================================
============================================================================*/

class ProxyModel : public QSortFilterProxyModel
{
public:
    explicit ProxyModel(QObject *parent = 0);
public:
    void setPreventAll(bool b);
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
private:
    bool prevent;
};

/*============================================================================
================================ Static global functions =====================
============================================================================*/

static QString chooseDir(const QString &path)
{
    if (path.isEmpty())
        return "";
    QFileInfo fi(path);
    return fi.isDir() ? QDir::cleanPath(fi.absoluteFilePath()) : QString();
}

/*============================================================================
================================ ProxyModel ==================================
============================================================================*/

ProxyModel::ProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    prevent = false;
}

void ProxyModel::setPreventAll(bool b)
{
    if (b == prevent)
        return;
    prevent = b;
    invalidateFilter();
}

bool ProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    return !prevent && QSortFilterProxyModel::filterAcceptsRow(row, parent);
}

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle, const QString &topDir) :
    BBasePrivate(q), CmboxStyle(comboBoxStyle), TopDir(chooseDir(topDir))
{
    //
}

BFileDialogPrivate::~BFileDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BFileDialogPrivate::init()
{
    maxHistorySize = 20;
    B_Q(BFileDialog);
    if (!TopDir.isEmpty())
    {
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
        proxy = new ProxyModel(this);
        view->setModel(proxy);
        proxy->setSourceModel(mdl);
        q->findChild<QWidget *>("lookInCombo")->setEnabled(false);
        q->findChild<QWidget *>("sidebar")->setEnabled(false);
        checkGoToParent();
    }
    else
    {
        proxy = 0;
    }
    connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(checkHistory()));
    q->setOption(BFileDialog::DontUseNativeDialog, true);
    lt = q->layout();
      lblEncodings = new QLabel(q);
        lblEncodings->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(lblEncodings);
      cmboxEncodings = new BTextCodecComboBox(CmboxStyle);
      connect(cmboxEncodings, SIGNAL(codecChanged(QTextCodec *)), q, SIGNAL(codecChanged(QTextCodec *)));
      connect(cmboxEncodings, SIGNAL(codecNameChanged(QString)), q, SIGNAL(codecNameChanged(QString)));
      lt->addWidget(cmboxEncodings);
}

bool BFileDialogPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return false;
    int key = static_cast<QKeyEvent *>(e)->key();
    if (o->objectName() == "listView" || o->objectName() == "treeView")
    {
        return (Qt::Key_Backspace == key && !pathFits(q_func()->directory().absolutePath()));
    }
    else
    {
        if (Qt::Key_Return != key && Qt::Key_Enter != key)
            return false;
        QString text = q_func()->findChild<QLineEdit *>("fileNameEdit")->text();
        QString path = QDir::cleanPath(q_func()->directory().absolutePath() +
                                       (text.startsWith("/") ? "" : "/") + text);
        bool a = QDir(text).isAbsolute();
        return !((!a && pathFits(path)) || (a && pathFits(text)));
    }
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
    QFileDialog(parent), BBase(*new BFileDialogPrivate(this, BTextCodecMenu::StructuredStyle))
{
    d_func()->init();
}

BFileDialog::BFileDialog(const QString &topDir, QWidget *parent) :
    QFileDialog(parent, "", chooseDir(topDir)),
    BBase(*new BFileDialogPrivate(this, BTextCodecMenu::StructuredStyle, topDir))
{
    d_func()->init();
}

BFileDialog::BFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent) :
    QFileDialog(parent), BBase(*new BFileDialogPrivate(this, comboBoxStyle))
{
    d_func()->init();
}

BFileDialog::BFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent) :
    QFileDialog(parent, "", chooseDir(topDir)), BBase(*new BFileDialogPrivate(this, comboBoxStyle, topDir))
{
    d_func()->init();
}

BFileDialog::~BFileDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BFileDialog::BFileDialog(BFileDialogPrivate &d, QWidget *parent) :
    QFileDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BFileDialog::setMaxHistorySize(int sz)
{
    if (sz < 1)
        return;
    d_func()->maxHistorySize = sz;
    d_func()->checkHistory();
}

void BFileDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    QFileDialog::restoreState(m.value("q_file_dialog_state").toByteArray());
    d_func()->checkHistory();
    selectCodec(m.value("codec_name").toString());
}

int BFileDialog::maxHistorySize() const
{
    return d_func()->maxHistorySize;
}

QString BFileDialog::topDir() const
{
    return d_func()->TopDir;
}

bool BFileDialog::codecSelectionEnabled() const
{
    return d_func()->cmboxEncodings->isVisible();
}

QTextCodec *BFileDialog::selectedCodec() const
{
    return d_func()->cmboxEncodings->selectedCodec();
}

QString BFileDialog::selectedCodecName() const
{
    return d_func()->cmboxEncodings->selectedCodecName();
}

QByteArray BFileDialog::saveState() const
{
    QVariantMap m;
    m.insert("q_file_dialog_state", QFileDialog::saveState());
    m.insert("codec_name", selectedCodecName());
    return BeQt::serialize(m);
}

/*============================== Public slots ==============================*/

void BFileDialog::setCodecSelectionEnabled(bool b)
{
    d_func()->cmboxEncodings->setVisible(b);
}

void BFileDialog::selectCodec(QTextCodec *codec)
{
    d_func()->cmboxEncodings->selectCodec(codec);
}

void BFileDialog::selectCodec(const QString &codecName)
{
    d_func()->cmboxEncodings->selectCodec(codecName);
}
