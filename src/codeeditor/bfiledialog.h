#ifndef BFILEDIALOG_H
#define BFILEDIALOG_H

class BFileDialogPrivate;
class BAbstractFileType;

class QString;
class QTextCodec;
class QByteArray;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QFileDialog>
#include <QList>

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

#endif // BFILEDIALOG_H
