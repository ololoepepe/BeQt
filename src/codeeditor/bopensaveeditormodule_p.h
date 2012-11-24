#ifndef BOPENSAVEEDITORMODULE_P_H
#define BOPENSAVEEDITORMODULE_P_H

class BFileDialogPrivate;
class BAbstractFileType;

class QString;
class QTextCodec;
class QByteArray;
class QLayout;
class QComboBox;
class QLabel;

#include "bopensaveeditormodule.h"
#include "babstracteditormodule_p.h"

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

class BFileDialog : public QFileDialog, public BBase
{
    B_DECLARE_PRIVATE(BFileDialog)
    Q_OBJECT
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
private:
    Q_DISABLE_COPY(BFileDialog)
};

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

/*============================================================================
================================ Open Save Editor Module Private
============================================================================*/

class BOpenSaveEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BOpenSaveEditorModule)
    B_DECLARE_TR_FUNCTIONS(BOpenSaveEditorModule, q)
    Q_OBJECT
    Q_DISABLE_COPY(BOpenSaveEditorModulePrivate)
public:
    explicit BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q);
    ~BOpenSaveEditorModulePrivate();
    //
    void init();
};

#endif // BOPENSAVEEDITORMODULE_P_H

