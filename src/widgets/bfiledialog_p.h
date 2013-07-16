#ifndef BFILEDIALOG_P_H
#define BFILEDIALOG_P_H

class QLayout;
class QLabel;
class QComboBox;
class QEvent;

#include "bfiledialog.h"
#include "btextcodecmenu.h"
#include "btextcodeccombobox.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BFileDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BFileDialog)
public:
    explicit BFileDialogPrivate(BFileDialog *q, BTextCodecMenu::Style comboBoxStyle);
    ~BFileDialogPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    bool pathFits(const QString &path) const;
public Q_SLOTS:
    void checkHistory();
    void checkGoToParent();
    void checkLineEdit(const QString &text);
public:
    const BTextCodecMenu::Style CmboxStyle;
public:
    int maxHistorySize;
    QString topDir;
    QLayout *lt;
      QLabel *lblEncodings;
      BTextCodecComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_H
