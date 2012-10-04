#ifndef BSELECTFILESDIALOG_H
#define BSELECTFILESDIALOG_H

class BTextEditorDocument;

class QVBoxLayout;
class QLabel;
class QListWidget;
class QHBoxLayout;
class QPushButton;

#include <QDialog>
#include <QList>
#include <QString>

class BSelectFilesDialog : public QDialog
{
    Q_OBJECT
public:
    enum Decision
    {
        DiscardDecision,
        CancelDecision,
        SaveDecision
    };
    //
    explicit BSelectFilesDialog(const QList<BTextEditorDocument *> &documents, QWidget *parent = 0);
    //
    Decision decision() const;
    QList<BTextEditorDocument *> selectedDocuments() const;
private:
    QList<BTextEditorDocument *> _m_documents;
    Decision _m_decision;
    //
    QVBoxLayout *_m_vlt;
      QLabel *_m_lblText;
      QLabel *_m_lblInformativeTextUpper;
      QListWidget *_m_lstwgt;
      QLabel *_m_lblInformativeTextLower;
      QHBoxLayout *_m_hlt;
        //stretch
        QPushButton *_m_btnDiscard;
        QPushButton *_m_btnCancel;
        QPushButton *_m_btnSave;
private slots:
    void _m_btnDiscardClicked();
    void _m_btnCancelClicked();
    void _m_btnSaveClicked();
};

#endif // BSELECTFILESDIALOG_H
