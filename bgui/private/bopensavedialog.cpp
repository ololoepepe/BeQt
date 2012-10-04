class QWidget;

#include "bopensavedialog.h"

#include <QString>
#include <QFileDialog>
#include <QMap>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QVariant>
#include <QStringList>

BOpenSaveDialog::BOpenSaveDialog(Mode mode, const QMap<QString, QString> &encodings,
                                 const QString &codecName, QWidget *parent) :
    QFileDialog(parent)
{
    setOption(DontUseNativeDialog);
    setFileMode(AnyFile);
    if (OpenMode == mode)
    {
        setWindowTitle( tr("Select file", "windowTitle") );
        setAcceptMode(AcceptOpen);
    }
    else
    {
        setWindowTitle( tr("Select file name", "windowTitle") );
        setAcceptMode(AcceptSave);
    }
    QLayout *lt = layout();
      _m_lblEncoding = new QLabel(this);
        _m_lblEncoding->setText(tr("Encoding", "lbl text") + ":");
      lt->addWidget(_m_lblEncoding);
      _m_cmboxEncoding = new QComboBox(this);
        QStringList sl = encodings.keys();
        for (int i = 0; i < sl.size(); ++i)
        {
            const QString &cn = sl.at(i);
            _m_cmboxEncoding->addItem(encodings.value(cn), cn);
        }
        int index = _m_cmboxEncoding->findData(codecName);
        if (index >= 0)
            _m_cmboxEncoding->setCurrentIndex(index);
      lt->addWidget(_m_cmboxEncoding);
}

//

QString BOpenSaveDialog::codecName() const
{
    return _m_cmboxEncoding->itemData( _m_cmboxEncoding->currentIndex() ).toString();
}
