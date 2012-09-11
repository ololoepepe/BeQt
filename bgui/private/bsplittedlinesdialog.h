#ifndef BSPLITTEDLINESDIALOG_H
#define BSPLITTEDLINESDIALOG_H

class QWidget;
class QVBoxLayout;
class QLabel;
class QListWidget;
class QHBoxLayout;
class QPushButton;
class QListWidgetItem;
class QCheckBox;

#include <QDialog>
#include <QList>

class BSplittedLinesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BSplittedLinesDialog(const QList<int> &lineIndexes, QWidget *parent = 0);
private:
    QVBoxLayout *_m_vlt;
      QLabel *_m_lbl;
      QListWidget *_m_lstwgt;
      QCheckBox *_m_cbox;
      QHBoxLayout *_m_hlt;
        //stretch
        QPushButton *_m_btnGoto;
        QPushButton *_m_btnClose;
private slots:
    void _m_btnGotoClicked();
    void _m_lstwgtItemDoubleClicked(QListWidgetItem *item);
signals:
    void gotoLine(int index, bool);
};

#endif // BSPLITTEDLINESDIALOG_H
