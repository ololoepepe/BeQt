#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BAbstractCodeEditorDocument;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BSelectDocumentsDialogPrivate;
class BSplittedLinesDialogPrivate;
class BAbstractFileType;
class BCodeEditorDocument;
class BSpellChecker;

class QVBoxLayout;
class QTabWidget;
class QPoint;
class QLabel;
class QListWidget;
class QDialogButtonBox;
class QAbstractButton;
class QTextCodec;
class QEvent;
class QCheckBox;
class QPushButton;
class QListWidgetItem;

#include "bcodeeditor.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QList>
#include <QDialog>
#include <QFont>
#include <QStringList>

/*============================================================================
================================ BSelectDocumentsDialog ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSelectDocumentsDialog)
public:
    explicit BSelectDocumentsDialog(const QList<BAbstractCodeEditorDocument *> &documents, QWidget *parent = 0);
    ~BSelectDocumentsDialog();
protected:
    explicit BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent = 0);
public:
    int decision() const;
    QList<BAbstractCodeEditorDocument *> selectedDocuments() const;
private:
    Q_DISABLE_COPY(BSelectDocumentsDialog)
};

/*============================================================================
================================ BSelectDocumentsDialogPrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSelectDocumentsDialog)
public:
    explicit BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q,
                                           const QList<BAbstractCodeEditorDocument *> &list);
    ~BSelectDocumentsDialogPrivate();
public:
    void init();
public Q_SLOTS:
    void dlgbboxClicked(QAbstractButton *button);
public:
    const QList<BAbstractCodeEditorDocument *> Documents;
public:
    int decision;
    QVBoxLayout *vlt;
      QLabel *lblText;
      QLabel *lblInformativeTextUpper;
      QListWidget *lstwgt;
      QLabel *lblInformativeTextLower;
      QDialogButtonBox *dlgbbox;
        //Save
        //Discard
        //Cancel
private:
    Q_DISABLE_COPY(BSelectDocumentsDialogPrivate)
};

/*============================================================================
================================ BSplittedLinesDialog ========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSplittedLinesDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSplittedLinesDialog)
public:
    explicit BSplittedLinesDialog(const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength,
                                  QWidget *parent = 0);
    ~BSplittedLinesDialog();
protected:
    explicit BSplittedLinesDialog(BSplittedLinesDialogPrivate &d, QWidget *parent = 0);
Q_SIGNALS:
    void gotoLine(const QPoint &pos);
    void selectLines(const QPoint &start, const QPoint &end);
private:
    Q_DISABLE_COPY(BSplittedLinesDialog)
};

/*============================================================================
================================ BSplittedLinesDialogPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BSplittedLinesDialogPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSplittedLinesDialog)
public:
    explicit BSplittedLinesDialogPrivate(BSplittedLinesDialog *q,
                                         const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength);
    ~BSplittedLinesDialogPrivate();
public:
    static QListWidgetItem *createListWidgetItem(const BCodeEdit::SplittedLinesRange &range, int lineLength);
public:
    void init();
public Q_SLOTS:
    void lstwgtItemDoubleClicked(QListWidgetItem *item);
    void btnGotoClicked();
public:
    const QList<BCodeEdit::SplittedLinesRange> Ranges;
    const int LineLength;
public:
    QVBoxLayout *vlt;
      QLabel *lbl;
      QListWidget *lstwgt;
      QCheckBox *cboxSelect;
      QDialogButtonBox *dlgbbox;
        QPushButton *btnGoto;
        //Close
private:
    Q_DISABLE_COPY(BSplittedLinesDialogPrivate)
};

/*============================================================================
================================ BDropHandler ================================
============================================================================*/

class B_CODEEDITOR_EXPORT BDropHandler : public QObject
{
    Q_OBJECT
public:
    explicit BDropHandler(BCodeEditorPrivate *parent);
    ~BDropHandler();
public:
    bool eventFilter(QObject *o, QEvent *e);
private:
    BCodeEditorPrivate *const Editor;
private:
    Q_DISABLE_COPY(BDropHandler)
};

/*============================================================================
================================ BCloseHandler ===============================
============================================================================*/

class B_CODEEDITOR_EXPORT BCloseHandler : public QObject
{
    Q_OBJECT
public:
    explicit BCloseHandler(BCodeEditorPrivate *parent);
    ~BCloseHandler();
public:
    bool eventFilter(QObject *o, QEvent *e);
public Q_SLOTS:
    void processingFinished();
private:
    BCodeEditorPrivate *const Editor;
private:
    QObject *lastSender;
private:
    Q_DISABLE_COPY(BCloseHandler)
};

/*============================================================================
================================ BCodeEditorPrivate ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEditor)
public:
    enum Operation
    {
        AddOperation,
        OpenOperation,
        SaveOperation
    };
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
public:
    static QString defaultFileName();
    static QString createFileName(const QString &fileName, const QString &defaultName,
                                  const QStringList &existingNames);
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    bool tryAddFileType(BAbstractFileType *ft);
    bool tryRemoveFileType(const QString &id);
    bool findDocument(const QString &fileName);
    BAbstractCodeEditorDocument *createDocument( const QString &fileName = QString(), const QString &text = QString() );
    void addDocument(BAbstractCodeEditorDocument *doc);
    void removeDocument(BAbstractCodeEditorDocument *doc);
    BAbstractFileType *selectDocumentFileType(BAbstractCodeEditorDocument *doc);
    BAbstractCodeEditorDocument *openDocument(QString fileName, QTextCodec *codec = 0);
    bool reopenDocument(BAbstractCodeEditorDocument *doc, QTextCodec *codec = 0);
    bool saveDocument(BAbstractCodeEditorDocument *doc, const QString &newFileName = QString(), QTextCodec *codec = 0);
    bool saveDocuments(const QList<BAbstractCodeEditorDocument *> &list);
    bool closeDocument(BAbstractCodeEditorDocument *doc);
    bool closeAllDocuments(bool save = true);
    bool tryCloseDocument(BAbstractCodeEditorDocument *doc);
    bool isDefaultFileName(const QString &fileName) const;
    void updateDocumentTab(BAbstractCodeEditorDocument *doc);
    void appendFileHistory( const QString &fileName, const QString &oldFileName = QString() );
    void failedToOpenMessage(const QString &fileName, const QString &info = QString());
    void failedToSaveMessage( const QString &fileName, const QString &newFileName = QString() );
    void alreadyOpenedMessage(const QString &fileName);
    int reopenModifiedMessage(const QString &fileName);
    int closeModifiedMessage(const QString &fileName);
    void showClosingMessage(QWidget *parent);
    BSplittedLinesDialog *createSplittedLinesDialog(BCodeEditorDocument *doc,
                                                    const QList<BCodeEdit::SplittedLinesRange> ranges);
    void emitDefaultCodecChanged(const QString &codecName);
    void emitEditModeChanged(BCodeEdit::EditMode mode);
    void emitDocumentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    void emitDocumentAdded(BAbstractCodeEditorDocument *doc);
    void emitDocumentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    void emitDocumentRemoved(const QString &fileName);
    void emitCurrentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void emitCurrentDocumentFileNameChanged(const QString &fileName);
    void emitCurrentDocumentModificationChanged(bool modified);
    void emitFileTypesChanged();
    void emitFileHistoryChanged(const QStringList &list);
    void setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr);
    void setDriverEditor(BAbstractDocumentDriver *drv, BCodeEditor *edr);
public Q_SLOTS:
    void createDropHandler();
    void createCloseHandler();
    void twgtCurrentChanged(int index);
    void twgtTabCloseRequested(int index);
    void documentReadOnlyChanged(bool ro);
    void documentModificationChanged(bool modified);
    void documentSelectionChanged();
    void documentHasSelectionChanged(bool hasSelection);
    void documentCutAvailableChanged(bool available);
    void documentCopyAvailableChanged(bool available);
    void documentPasteAvailableChanged(bool available);
    void documentUndoAvailableChanged(bool available);
    void documentRedoAvailableChanged(bool available);
    void documentEditModeChanged(BCodeEdit::EditMode mode);
    void documentCursorPositionChanged(const QPoint &pos);
    void documentBuisyChanged(bool buisy);
    void documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
    void documentFileNameChanged(const QString &fn);
    void documentCodecChanged(const QString &codecName);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void documentLoadingFinished(bool success);
    void documentSavingFinished(bool success);
public:
    int docType;
    QMap<QString, BAbstractEditorModule *> modules;
    BAbstractCodeEditorDocument *document;
    QMap<BAbstractCodeEditorDocument *, QString> openingDocuments;
    QMap<BAbstractCodeEditorDocument *, QString> savingDocuments;
    QList<BAbstractCodeEditorDocument *> closingDocuments;
    QList<BAbstractCodeEditorDocument *> processedDocuments;
    QFont editFont;
    BCodeEdit::EditMode editMode;
    int editLineLength;
    BeQt::TabWidth editTabWidth;
    bool lineNumberVisible;
    bool bracketsHighlighting;
    BSpellChecker *spellChecker;
    QTextCodec *defaultCodec;
    QString defaultFN;
    int maximumFileSize;
    int asyncMin;
    BAbstractDocumentDriver *driver;
    QMap<QString, BAbstractFileType *> fileTypes;
    BAbstractFileType *preferredFileType;
    BDropHandler *dropHandler;
    BCloseHandler *closeHandler;
    QStringList fileHistory;
    int maxHistoryCount;
    QVBoxLayout *vlt;
      QTabWidget *twgt;
private:
    friend class BDropHandler;
    friend class BCloseHandler;
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
};

#endif // BCODEEDITOR_P_H
