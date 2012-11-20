#ifndef BSEARCHDIALOG_P_H
#define BSEARCHDIALOG_P_H

class BCodeEditorDocument;

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QGroupBox;
class QCheckBox;
class QDialogButtonBox;
class QPushButton;
class QMenu;
class QAction;
class QString;
class QEvent;
class QString;

#include "bsearchdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QDataStream>
#include <QTextDocument>

/*============================================================================
================================ Search Dialog Private
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSearchDialog)
    B_DECLARE_TR_FUNCTIONS(BSearchDialog, q)
    Q_OBJECT
public:
    explicit BSearchDialogPrivate(BSearchDialog *q);
    ~BSearchDialogPrivate();
    //
    bool eventFilter(QObject *o, QEvent *e);
    void appendHistory(QComboBox *cmbox);
    void emitTextReplaced(int count, const QString &oldText, const QString &newText);
    QTextDocument::FindFlags createFindFlags() const;
    QString windowTitle() const;
    //
    static const int DefMaximumHistorySize;
    static const QDataStream::Version DSVersion;
    //
    BCodeEditorDocument *document;
    //
    QVBoxLayout *vlt;
      QHBoxLayout *hltSearch;
        QLabel *lblSearch;
        QComboBox *cmboxSearch;
      QHBoxLayout *hltReplace;
        QLabel *lblReplace;
        QComboBox *cmboxReplace;
      QGroupBox *gboxOptions;
        QVBoxLayout *vltOptions;
          QCheckBox *cboxCaseSensitive;
          QCheckBox *cboxWholeWords;
          QCheckBox *cboxBackwardOrder;
          QCheckBox *cboxCyclic;
      QDialogButtonBox *dlgbbox;
        //Close
        QPushButton *btnReplaceAll;
          QMenu *mnuReplaceAll;
            QAction *actSelection;
            QAction *actDocument;
        QPushButton *btnReplace;
        QPushButton *btnFind;
public slots:
    void retranslateUi();
    void checkSearchReplace();
    void actSelectionTriggered();
    void actDocumentTriggered();
private:
    Q_DISABLE_COPY(BSearchDialogPrivate)
};

#endif // BSEARCHDIALOG_P_H

