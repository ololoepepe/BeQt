#ifndef BFINDDIALOG_H
#define BFINDDIALOG_H

class QWidget;
class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QGroupBox;
class QCheckBox;
class QPushButton;
class QMenu;
class QAction;
class QShowEvent;

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QTextDocument>

class BFindDialog : public QDialog
{
    Q_OBJECT
public:
    struct Parameters
    {
        QStringList textHistory;
        QStringList newTextHistory;
        bool caseSensitive;
        bool wholeWords;
        bool backwardOrder;
        bool cyclic;
    };
    //
    explicit BFindDialog(QWidget *parent = 0);
    //
    void setDocumentAvailable(bool available);
    void setSelectionAvailable(bool available);
    void setText(const QString &text);
    void setReplaceAvailable(bool available);
    void setLineLength(int length);
    void setParameters(const Parameters &param);
    Parameters parameters() const;
    QString text() const;
    QString newText() const;
    Qt::CaseSensitivity caseSensitivity() const;
    QTextDocument::FindFlags findFlags() const;
    bool cyclic() const;
protected:
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);
private:
    bool _m_documentAvailable;
    bool _m_selectionAvailable;
    bool _m_replaceAvailable;
    //
    QVBoxLayout *_m_vlt;
      QHBoxLayout *_m_hltText;
        QLabel *_m_lblText;
        QComboBox *_m_cmboxText;
      QHBoxLayout *_m_hltNewText;
        QLabel *_m_lblNewText;
        QComboBox *_m_cmboxNewText;
      QGroupBox *_m_gboxOptions;
        QVBoxLayout *_m_vltOptions;
          QCheckBox *_m_cboxCaseSensitive;
          QCheckBox *_m_cboxWholeWords;
          QCheckBox *_m_cboxBackwardOrder;
          QCheckBox *_m_cboxCyclic;
      QHBoxLayout *_m_hltActions;
        //stretch
        QPushButton *_m_btnClose;
        QPushButton *_m_btnReplaceAll;
          QMenu *_m_mnuReplaceAll;
            QAction *_m_actSelection;
            QAction *_m_actDocument;
            QAction *_m_actAllDocuments;
        QPushButton *_m_btnReplace;
        QPushButton *_m_btnFind;
    //
    void _m_retranslateUi();
    void _m_check();
    QStringList _m_textHistory() const;
    QStringList _m_newTextHistory() const;
private slots:
    void _m_cmboxTextEditTextChanged(const QString &text);
    void _m_appendTextHistory();
    void _m_appendNewTextHistory();
signals:
    void findNextAvailableChanged(bool available);
    void replaceNextAvailableChanged(bool available);
    void findNext();
    void replaceNext();
    void replaceInSelection();
    void replaceInDocument();
    void replaceInAllDocuments();
};

#endif // BFINDDIALOG_H
