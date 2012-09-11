#ifndef BOPENSAVEDIALOG_H
#define BOPENSAVEDIALOG_H

class BEncoding;

class QWidget;
class QLabel;
class QComboBox;

#include <QFileDialog>
#include <QList>
#include <QString>
#include <QRect>

class BOpenSaveDialog : public QFileDialog
{
    Q_OBJECT
public:
    enum Mode
    {
        OpenMode,
        SaveMode
    };
    //
    BOpenSaveDialog(const QString &id, Mode mode, const QList<BEncoding *> &encodings,
                    const QString &codecName, QWidget *parent = 0);
    //
    QString codecName() const;
private:
    const QString _m_CId;
    //
    //layout
      QLabel *_m_lblEncoding;
      QComboBox *_m_cmboxEncoding;
    //
    void _m_loadSettings();
private slots:
    void _m_saveSettings();
};

#endif // BOPENSAVEDIALOG_H
