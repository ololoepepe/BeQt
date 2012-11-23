#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorDocument;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BSelectDocumentsDialogPrivate;
class BAbstractFileType;

class QVBoxLayout;
class QTabWidget;
class QString;
class QPoint;
class QFont;
class QLabel;
class QListWidget;
class QDialogButtonBox;
class QAbstractButton;

#include "bcodeeditor.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QString>
#include <QList>
#include <QDialog>

/*============================================================================
================================ Select Documents Dialog
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialog : public QDialog, public BBase
{
    B_DECLARE_PRIVATE(BSelectDocumentsDialog)
    Q_OBJECT
public:
    explicit BSelectDocumentsDialog(const QList<BCodeEditorDocument *> &documents, QWidget *parent = 0);
    ~BSelectDocumentsDialog();
    //
    int decision() const;
    QList<BCodeEditorDocument *> selectedDocuments() const;
protected:
    BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BSelectDocumentsDialog)
};

/*============================================================================
================================ Select Documents Dialog Private
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSelectDocumentsDialog)
    B_DECLARE_TR_FUNCTIONS(BSelectDocumentsDialog, q)
    Q_OBJECT
public:
    explicit BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q, const QList<BCodeEditorDocument *> &list);
    ~BSelectDocumentsDialogPrivate();
    //
    void init();
    //
    const QList<BCodeEditorDocument *> Documents;
    //
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
private slots:
    void dlgbboxClicked(QAbstractButton *button);
private:
    Q_DISABLE_COPY(BSelectDocumentsDialogPrivate)
};

/*============================================================================
================================ Code Editor Private
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditor)
    B_DECLARE_TR_FUNCTIONS(BCodeEditor, q)
    Q_OBJECT
public:
    enum Operation
    {
        AddOperation,
        OpenOperation,
        SaveOperation
    };
    //
    static QString defaultFileName();
    static QString createFileName(const QString &fileName);
    static bool waitForAll(const QMap<BCodeEditorDocument *, QString> &what, BCodeEditor *where,
                           const char *untilSignal, int msecs);
    //
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
    //
    void init();
    bool tryAddFileType(BAbstractFileType *ft);
    bool tryRemoveFileType(const QString &id);
    bool findDocument(const QString &fileName);
    BCodeEditorDocument *createDocument( const QString &fileName = QString(), const QString &text = QString() );
    void addDocument(BCodeEditorDocument *doc);
    void removeDocument(BCodeEditorDocument *doc);
    BAbstractFileType *selectDocumentFileType(BCodeEditorDocument *doc);
    bool openDocument(const QString &fileName);
    bool saveDocument( BCodeEditorDocument *doc, const QString &newFileName = QString() );
    bool saveDocuments(const QList<BCodeEditorDocument *> &list);
    bool closeDocument(BCodeEditorDocument *doc);
    bool closeAllDocuments();
    bool tryCloseDocument(BCodeEditorDocument *doc);
    void updateDocumentTab(BCodeEditorDocument *doc);
    //Messages
    void failedToOpenMessage(const QString &fileName);
    void failedToSaveMessage( const QString &fileName, const QString &newFileName = QString() );
    int closeModifiedMessage(const QString &fileName);
    //Signal emitting
    void emitDocumentAboutToBeAdded(BCodeEditorDocument *doc);
    void emitDocumentAdded(BCodeEditorDocument *doc);
    void emitDocumentAboutToBeRemoved(BCodeEditorDocument *doc);
    void emitCurrentDocumentChanged(BCodeEditorDocument *doc);
    void emitFileTypesChanged();
    void emitAllDocumentsOpened();
    void emitAllDocumentsSaved();
    void emitAllDocumentsClosed();
    //External private class call
    void setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr);
    //
    QMap<QString, BAbstractEditorModule *> modules;
    BCodeEditorDocument *document;
    QMap<BCodeEditorDocument *, QString> openingDocuments;
    QMap<BCodeEditorDocument *, QString> savingDocuments;
    QMap<BCodeEditorDocument *, QString> closingDocuments;
    QFont editFont;
    BCodeEdit::EditMode editMode;
    int editLineLength;
    BCodeEdit::TabWidth editTabWidth;
    bool bracketsHighlighting;
    BAbstractDocumentDriver *driver;
    QMap<QString, BAbstractFileType *> fileTypes;
    BAbstractFileType *defaultFileType;
    //
    QVBoxLayout *vlt;
      QTabWidget *twgt;
public slots:
    void twgtCurrentChanged(int index);
    void twgtTabCloseRequested(int index);
    //BCodeEdit events
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
    //BCodeEditorDocument events
    void documentFileNameChanged(const QString &fn);
    void documentCodecChanged(const QString &codecName);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void documentLoadingFinished(bool success);
    void documentSavingFinished(bool success);
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
};

#endif // BCODEEDITOR_P_H

