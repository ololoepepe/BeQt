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
================================ BFileDialog =================================
============================================================================*/

class B_CODEEDITOR_EXPORT BFileDialog : public QFileDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BFileDialog)
public:
    explicit BFileDialog(QWidget *parent = 0);
    ~BFileDialog();
protected:
    explicit BFileDialog(BFileDialogPrivate &d, QWidget *parent = 0);
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
private:
    Q_DISABLE_COPY(BFileDialog)
};

/*============================================================================
================================ BFileDialogPrivate ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BFileDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BFileDialog)
public:
    static const QDataStream::Version DSVersion;
public:
    explicit BFileDialogPrivate(BFileDialog *q);
    ~BFileDialogPrivate();
public:
    void init();
    void addEncoding(QTextCodec *codec);
public:
    QMap<QTextCodec *, int> codecIndexes;
    QList<BAbstractFileType *> fileTypes;
    QLayout *lt;
      QLabel *lblEncodings;
      QComboBox *cmboxEncodings;
private:
    Q_DISABLE_COPY(BFileDialogPrivate)
};

/*============================================================================
================================ BLocalDocumentDriverPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BAbstractDocumentDriverPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
public:
    void init();
public:
    QString defaultDir;
    bool nativeLineEnd;
    QByteArray fileDialogState;
    BAbstractFileType *lastFileType;
private:
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
};

#endif // BLOCALDOCUMENTDRIVER_P_H
