#ifndef BFILEDIALOG_P_H
#define BFILEDIALOG_P_H

class QLayout;
class QLabel;
class QComboBox;

#include "bfiledialog.h"
#include "btextcodecmenu.h"
#include "btextcodeccombobox.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

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
public Q_SLOTS:
    void updateHistory();
public:
    const BTextCodecMenu::Style CmboxStyle;
public:
    int maxHistorySize;
    QLayout *lt;
      QLabel *lblEncodings;
      BTextCodecComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_H
