#ifndef BSEARCHEDITORMODULE_P_H
#define BSEARCHEDITORMODULE_P_H

class BSearchDialogPrivate;
class BCodeEditorDocument;

class QWidget;
class QString;
class QByteArray;
class QStringList;
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

#include "bsearcheditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QDialog>
#include <QDataStream>
#include <QTextDocument>
#include <QPointer>

/*============================================================================
================================ Search Dialog
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchDialog : public QDialog, public BBase
{
    B_DECLARE_PRIVATE(BSearchDialog)
    Q_OBJECT
public:
    explicit BSearchDialog(QWidget *parent = 0);
    explicit BSearchDialog(bool replaceEnabled, QWidget *parent = 0);
    ~BSearchDialog();
    //
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setMatchWholeWords(bool enabled);
    void setBackwardOrder(bool enabled);
    void setCyclicSearch(bool enabled);
    void setSearchHistory(const QStringList &list);
    void setReplaceHistory(const QStringList &list);
    void setMaximumHistorySize(int sz);
    void setReplaceEnabled(bool enabled);
    void setDocument(BCodeEditorDocument *doc);
    void restoreState(const QByteArray &ba);
    Qt::CaseSensitivity caseSensitivity() const;
    bool matchWholeWords() const;
    bool backwardOrder() const;
    bool cyclicSearch() const;
    QStringList searchHistory() const;
    QStringList replaceHistory() const;
    int maximumHistorySize() const;
    bool replaceEnabled() const;
    BCodeEditorDocument *document() const;
    bool findNextAvailable() const;
    bool replaceNextAvailable() const;
    QByteArray saveState() const;
public slots:
    void findNext();
    void replaceNext();
signals:
    void findNextAvailableChanged(bool available);
    void replaceNextAvailableChanged(bool available);
    void textFound(bool found, const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
protected:
    BSearchDialog(BSearchDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BSearchDialog)
};

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
    void init();
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
            QAction *actAllDocuments;
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

/*============================================================================
================================ Search Editor Module Private
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BSearchEditorModule)
    B_DECLARE_TR_FUNCTIONS(BSearchEditorModule, q)
    Q_OBJECT
public:
    explicit BSearchEditorModulePrivate(BSearchEditorModule *q);
    ~BSearchEditorModulePrivate();
    //
    void init();
    void setDialogParent(QWidget *parent = 0);
    //
    BSearchDialog *sdlg;
    QPointer<QAction> actFind;
    QPointer<QAction> actFindNext;
public slots:
    void retranslateUi();
private:
    Q_DISABLE_COPY(BSearchEditorModulePrivate)
};

#endif // BSEARCHEDITORMODULE_P_H

