#ifndef BEXTENDEDFILEDIALOG_H
#define BEXTENDEDFILEDIALOG_H

class BExtendedFileDialogPrivate;
class BAbstractFileType;

class QString;
class QTextCodec;
class QByteArray;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtWidgets/BTextCodecMenu>

#include <QObject>
#include <QFileDialog>
#include <QList>

/*============================================================================
================================ BExtendedFileDialog =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BExtendedFileDialog : public QFileDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BExtendedFileDialog)
public:
    explicit BExtendedFileDialog(QWidget *parent = 0);
    explicit BExtendedFileDialog(QWidget *parent, BTextCodecMenu::Style comboBoxStyle);
    ~BExtendedFileDialog();
protected:
    explicit BExtendedFileDialog(BExtendedFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void selectFileType(BAbstractFileType *ft);
    void selectFileType(const QString &id);
    void setCodecSelectionEnabled(bool b);
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
    void restoreState(const QByteArray &ba, bool includeGeometry = true);
    BAbstractFileType *selectedFileType() const;
    QString selectedFileTypeId() const;
    bool codecSelectionEnabled() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    QByteArray saveState(bool includeGeometry = true) const;
private:
    Q_DISABLE_COPY(BExtendedFileDialog)
};

#endif // BEXTENDEDFILEDIALOG_H
