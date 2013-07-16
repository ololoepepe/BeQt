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
#include <QSortFilterProxyModel>
#include <QModelIndex>

#include <QDebug>

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BFileDialogPrivate::BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle) :
    BBasePrivate(q), CmboxStyle(comboBoxStyle)
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
    connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(checkHistory()));
    connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(checkGoToParent()));
    connect(q->findChild<QToolButton *>("backButton"), SIGNAL(clicked()), this, SLOT(checkGoToParent()));
    connect(q->findChild<QToolButton *>("forwardButton"), SIGNAL(clicked()), this, SLOT(checkGoToParent()));
    connect(q->findChild<QLineEdit *>("fileNameEdit"), SIGNAL(textChanged(QString)),
            this, SLOT(checkLineEdit(QString)));
    q->findChild<QLineEdit *>("fileNameEdit")->installEventFilter(this);
    q->findChild<QWidget *>("listView")->installEventFilter(this);
    q->findChild<QWidget *>("treeView")->installEventFilter(this);
    q->findChild<QLineEdit *>("fileNameEdit")->completer()->popup()->installEventFilter(this);
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
        QString path = QDir::cleanPath(q_func()->directory().absolutePath() + (text.startsWith("/") ? "" : "/") + text);
        bool a = QDir(text).isAbsolute();
        return !((!a && pathFits(path)) || (a && pathFits(text)));
    }
}

bool BFileDialogPrivate::pathFits(const QString &path) const
{
    return topDir.isEmpty() || (path.startsWith(topDir) && path.length() > topDir.length());
}

/*============================== Public slots ==============================*/

void BFileDialogPrivate::checkHistory()
{
    QStringList list = q_func()->history();
    while (list.size() > maxHistorySize)
        list.removeFirst();
    foreach (int i, bRangeR(list.size() - 1, 0))
        if (!pathFits(list.at(i)))
            list.removeAt(i);
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
    btn->setEnabled(btn->isEnabled() && pathFits(path));
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

BFileDialog::BFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent) :
    QFileDialog(parent), BBase(*new BFileDialogPrivate(this, comboBoxStyle))
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

void BFileDialog::setTopDir(const QString &path)
{
    if (path == d_func()->topDir)
        return;
    d_func()->topDir = path;
    if (!d_func()->pathFits(path))
    {
        setDirectory(d_func()->topDir);
        d_func()->checkHistory();
        d_func()->checkLineEdit(findChild<QLineEdit *>("fileNameEdit")->text());
    }
    else
    {
        QLineEdit *ledt = findChild<QLineEdit *>("fileNameEdit");
        ledt->setText(ledt->text());
    }
    findChild<QWidget *>("lookInCombo")->setEnabled(path.isEmpty());
    findChild<QWidget *>("sidebar")->setEnabled(path.isEmpty());
    d_func()->checkGoToParent();
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
    return d_func()->topDir;
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
