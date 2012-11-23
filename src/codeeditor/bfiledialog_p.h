#ifndef BFILEDIALOG_P_H
#define BFILEDIALOG_P_H

class BCodeEditor;

class QLayout;
class QComboBox;
class QLabel;
class QTextCodec;

#include "bfiledialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>

/*============================================================================
================================ File Dialog Private
============================================================================*/

class BFileDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BFileDialog)
    B_DECLARE_TR_FUNCTIONS(BFileDialog, q)
    Q_OBJECT
public:
    explicit BFileDialogPrivate(BFileDialog *q);
    ~BFileDialogPrivate();
    //
    void init();
    void addEncoding(QTextCodec *codec, BCodeEditor *editor = 0);
    //
    QMap<QString, QTextCodec *> encodings;
    //
    QLayout *lt;
      QLabel *lblEncodings;
      QComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_P_H

