#ifndef BSETTINGSDIALOG_H
#define BSETTINGSDIALOG_H

class BAbstractSettingsTab;

class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTabWidget;

#include <QDialog>
#include <QMap>
#include <QVariantMap>

class BSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BSettingsDialog(const QMap<QString, BAbstractSettingsTab *> &tabMap, QWidget *parent = 0);
    //
    QMap<QString, QVariantMap> valueMapMap() const;
private:
    QMap<QString, BAbstractSettingsTab *> _m_tabMap;
    QVBoxLayout *_m_vlt;
      QTabWidget *_m_tabWgt;
      QHBoxLayout *_m_hltActions;
        //stretch
        QPushButton *_m_btnCancel;
        QPushButton *_m_btnOk;
    //
    void _m_retranslateUi();
};

#endif // BSETTINGSDIALOG_H
