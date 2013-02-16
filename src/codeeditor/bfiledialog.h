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
    enum CodecsComboBoxStyle
    {
        StructuredStyle = 0,
        PlainStyle
    };
public:
    explicit BFileDialog(QWidget *parent = 0);
    explicit BFileDialog(QWidget *parent, CodecsComboBoxStyle cmboxStyle);
    ~BFileDialog();
protected:
    explicit BFileDialog(BFileDialogPrivate &d, QWidget *parent = 0);
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
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H
