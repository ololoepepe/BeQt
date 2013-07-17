#ifndef BEXTENDEDFILEDIALOG_H
#define BEXTENDEDFILEDIALOG_H

class BExtendedFileDialogPrivate;
class BAbstractFileType;

class QString;
class QByteArray;

#include <BeQtCore/BeQtGlobal>
#include <BeQtWidgets/BTextCodecMenu>
#include <BeQtWidgets/BFileDialog>

#include <QObject>
#include <QList>

/*============================================================================
================================ BExtendedFileDialog =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialog : public BFileDialog
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BExtendedFileDialog)
public:
    explicit BExtendedFileDialog(QWidget *parent = 0);
    explicit BExtendedFileDialog(const QString &topDir, QWidget *parent = 0);
    explicit BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent = 0);
    explicit BExtendedFileDialog(BTextCodecMenu::Style comboBoxStyle, const QString &topDir, QWidget *parent = 0);
    ~BExtendedFileDialog();
protected:
    explicit BExtendedFileDialog(BExtendedFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void selectFileType(BAbstractFileType *ft);
    void selectFileType(const QString &id);
    void restoreState(const QByteArray &ba);
    BAbstractFileType *selectedFileType() const;
    QString selectedFileTypeId() const;
    QByteArray saveState() const;
private:
    Q_DISABLE_COPY(BExtendedFileDialog)
};

#endif // BEXTENDEDFILEDIALOG_H
