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
    connect(q, SIGNAL(directoryEntered(QString)), this, SLOT(updateHistory()));
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

/*============================== Public slots ==============================*/

void BFileDialogPrivate::updateHistory()
{
    QStringList list = q_func()->history();
    while (list.size() > maxHistorySize)
        list.removeFirst();
    q_func()->setHistory(list);
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
    QStringList list = history();
    while (list.size() > sz)
        list.removeFirst();
    setHistory(list);
}

void BFileDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    QFileDialog::restoreState(m.value("q_file_dialog_state").toByteArray());
    QStringList list = history();
    while (list.size() > d_func()->maxHistorySize)
        list.removeFirst();
    setHistory(list);
    selectCodec(m.value("codec_name").toString());
}

int BFileDialog::maxHistorySize() const
{
    return d_func()->maxHistorySize;
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
