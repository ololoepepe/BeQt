#ifndef BFILEDIALOG_P_H
#define BFILEDIALOG_P_H

class BAbstractFileType;

class QLayout;
class QComboBox;
class QLabel;
class QTextCodec;

#include "bfiledialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>
#include <QDataStream>

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
    void addEncoding(QTextCodec *codec);
    //
    static const QDataStream::Version DSVersion;
    //
    QMap<QTextCodec *, int> codecIndexes;
    QList<BAbstractFileType *> fileTypes;
    //
    QLayout *lt;
      QLabel *lblEncodings;
      QComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

#endif // BFILEDIALOG_P_H

