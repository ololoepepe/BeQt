#ifndef BFILEDIALOG_H
#define BFILEDIALOG_H

class BFileDialogPrivate;
class BAbstractFileType;
class BCodeEditor;

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
    void setEncodings(const QList<QTextCodec *> &list, BCodeEditor *editor = 0);
    void setSelectedEncoding(QTextCodec *codec);
    void setSelectedEncoding(const QString &codecName);
    QTextCodec *selectedEncoding() const;
    QString selectedCodecName() const;
private:
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H

