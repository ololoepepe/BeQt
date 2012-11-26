#ifndef BLOCALDOCUMENTDRIVER_P_H
#define BLOCALDOCUMENTDRIVER_P_H

class BFileDialogPrivate;
class BAbstractFileType;

class QString;
class QTextCodec;
class QByteArray;
class QLayout;
class QComboBox;
class QLabel;

#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QFileDialog>
#include <QList>
#include <QMap>
#include <QDataStream>

/*============================================================================
================================ File Dialog
============================================================================*/

class B_CODEEDITOR_EXPORT BFileDialog : public QFileDialog, public BBase
{
    B_DECLARE_PRIVATE(BFileDialog)
    Q_OBJECT
    Q_DISABLE_COPY(BFileDialog)
public:
    explicit BFileDialog(QWidget *parent = 0);
    ~BFileDialog();
protected:
    BFileDialog(BFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void setCodecs(const QList<QTextCodec *> &list);
    void selectFileType(BAbstractFileType *ft);
    void selectFileType(const QString &id);
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
    void restoreState(const QByteArray &ba, bool includeGeometry = true);
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    BAbstractFileType *selectedFileType() const;
    QString selectedFileTypeId() const;
    QByteArray saveState(bool includeGeometry = true) const;
};

/*============================================================================
================================ File Dialog Private
============================================================================*/

class B_CODEEDITOR_EXPORT BFileDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BFileDialog)
    B_DECLARE_TR_FUNCTIONS(BFileDialog, q)
    Q_OBJECT
    Q_DISABLE_COPY(BFileDialogPrivate)
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
};

/*============================================================================
================================ Local Document Driver Private
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BAbstractDocumentDriverPrivate
{
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
    Q_OBJECT
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
    //
    void init();
private:
    QString defaultDir;
    bool nativeLineEnd;
    QByteArray fileDialogState;
};

#endif // BLOCALDOCUMENTDRIVER_P_H

