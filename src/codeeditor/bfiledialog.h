#ifndef BFILEDIALOG_H
#define BFILEDIALOG_H

class BFileDialogPrivate;
class BAbstractFileType;

class QString;
class QTextCodec;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QFileDialog>
#include <QList>

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
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    BAbstractFileType *selectedFileType() const;
    QString selectedFileTypeId() const;
private:
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H

