#ifndef BFILEDIALOG_H
#define BFILEDIALOG_H

class BFileDialogPrivate;

class QString;
class QTextCodec;
class QByteArray;

#include "btextcodecmenu.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QFileDialog>
#include <QList>

/*============================================================================
================================ BFileDialog =================================
============================================================================*/

class B_WIDGETS_EXPORT BFileDialog : public QFileDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BFileDialog)
public:
public:
    explicit BFileDialog(QWidget *parent = 0);
    explicit BFileDialog(BTextCodecMenu::Style comboBoxStyle, QWidget *parent = 0);
    ~BFileDialog();
protected:
    explicit BFileDialog(BFileDialogPrivate &d, QWidget *parent = 0);
public:
    void setMaxHistorySize(int sz);
    void setTopDir(const QString &path);
    void restoreState(const QByteArray &ba);
    bool codecSelectionEnabled() const;
    int maxHistorySize() const;
    QString topDir() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
    QByteArray saveState() const;
public Q_SLOTS:
    void setCodecSelectionEnabled(bool b);
    void selectCodec(QTextCodec *codec);
    void selectCodec(const QString &codecName);
Q_SIGNALS:
    void codecChanged(QTextCodec *codec);
    void codecNameChanged(const QString &codecName);
private:
    Q_DISABLE_COPY(BFileDialog)
};

#endif // BFILEDIALOG_H
