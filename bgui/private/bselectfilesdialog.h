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
#include <QRect>

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
    explicit BSelectFilesDialog(const QString &id, const QList<BTextEditorDocument *> &documents, QWidget *parent = 0);
    //
    Decision decision() const;
    QList<BTextEditorDocument *> selectedDocuments() const;
private:
    const QString _m_CId;
    //
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
    //
    void _m_loadSettings();
    void _m_saveSettings();
private slots:
    void _m_btnDiscardClicked();
    void _m_btnCancelClicked();
    void _m_btnSaveClicked();
};

#endif // BSELECTFILESDIALOG_H
