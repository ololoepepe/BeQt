#ifndef BOPENSAVEDIALOG_H
#define BOPENSAVEDIALOG_H

class QWidget;
class QLabel;
class QComboBox;

#include <QFileDialog>
#include <QString>
#include <QMap>

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
    BOpenSaveDialog(Mode mode, const QMap<QString, QString> &encodings, const QString &codecName, QWidget *parent = 0);
    //
    QString codecName() const;
private:
    //layout
      QLabel *_m_lblEncoding;
      QComboBox *_m_cmboxEncoding;
};

#endif // BOPENSAVEDIALOG_H
