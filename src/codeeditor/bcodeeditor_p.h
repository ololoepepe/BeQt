#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorDocument;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BSelectDocumentsDialogPrivate;
class BSplittedLinesDialogPrivate;
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
class QTextCodec;
class QStringList;
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
#include <QCoreApplication>
#include <QMap>
#include <QString>
#include <QList>
#include <QDialog>

/*============================================================================
================================ BSelectDocumentsDialog ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialog : public QDialog, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSelectDocumentsDialog)
public:
    explicit BSelectDocumentsDialog(const QList<BCodeEditorDocument *> &documents, QWidget *parent = 0);
    ~BSelectDocumentsDialog();
protected:
    explicit BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent = 0);
public:
    int decision() const;
    QList<BCodeEditorDocument *> selectedDocuments() const;
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
    explicit BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q, const QList<BCodeEditorDocument *> &list);
    ~BSelectDocumentsDialogPrivate();
public:
    void init();
public slots:
    void dlgbboxClicked(QAbstractButton *button);
public:
    const QList<BCodeEditorDocument *> Documents;
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
signals:
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
public slots:
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
    static const QStringList UnicodeCodecs;
    static const QStringList EastEuropeanCodecs;
    static const QStringList WestEuropeanCodecs;
    static const QStringList EastAsianCodecs;
    static const QStringList SouthEastSouthWestAsianCodecs;
    static const QStringList MiddleEastCodecs;
    static const QStringList SupportedCodecs;
    static const QMap<QTextCodec *, QString> CodecNames;
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
public:
    static QString defaultFileName();
    static QString createFileName(const QString &fileName, const QString &defaultName,
                                  const QStringList &existingNames);
    static QMap<QTextCodec *, QString> createCodecNamesMap();
    static QString codecDescriptiveName(const QString &codecName);
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    bool tryAddFileType(BAbstractFileType *ft);
    bool tryRemoveFileType(const QString &id);
    bool findDocument(const QString &fileName);
    BCodeEditorDocument *createDocument( const QString &fileName = QString(), const QString &text = QString() );
    void addDocument(BCodeEditorDocument *doc);
    void removeDocument(BCodeEditorDocument *doc);
    BAbstractFileType *selectDocumentFileType(BCodeEditorDocument *doc);
    bool openDocument(const QString &fileName, QTextCodec *codec = 0);
    bool reopenDocument(BCodeEditorDocument *doc, QTextCodec *codec = 0);
    bool saveDocument(BCodeEditorDocument *doc, const QString &newFileName = QString(), QTextCodec *codec = 0);
    bool saveDocuments(const QList<BCodeEditorDocument *> &list);
    bool closeDocument(BCodeEditorDocument *doc);
    bool closeAllDocuments();
    bool tryCloseDocument(BCodeEditorDocument *doc);
    void updateDocumentTab(BCodeEditorDocument *doc);
    void appendFileHistory( const QString &fileName, const QString &oldFileName = QString() );
    void failedToOpenMessage(const QString &fileName);
    void failedToSaveMessage( const QString &fileName, const QString &newFileName = QString() );
    int reopenModifiedMessage(const QString &fileName);
    int closeModifiedMessage(const QString &fileName);
    BSplittedLinesDialog *createSplittedLinesDialog(BCodeEditorDocument *doc,
                                                    const QList<BCodeEdit::SplittedLinesRange> ranges);
    void emitDefaultCodecChanged(const QString &codecName);
    void emitEditModeChanged(BCodeEdit::EditMode mode);
    void emitDocumentAboutToBeAdded(BCodeEditorDocument *doc);
    void emitDocumentAdded(BCodeEditorDocument *doc);
    void emitDocumentAboutToBeRemoved(BCodeEditorDocument *doc);
    void emitCurrentDocumentChanged(BCodeEditorDocument *doc);
    void emitCurrentDocumentFileNameChanged(const QString &fileName);
    void emitCurrentDocumentModificationChanged(bool modified);
    void emitFileTypesChanged();
    void emitFileHistoryChanged(const QStringList &list);
    void setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr);
    void setDriverEditor(BAbstractDocumentDriver *drv, BCodeEditor *edr);
public slots:
    void createDropHandler();
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
    QMap<QString, BAbstractEditorModule *> modules;
    BCodeEditorDocument *document;
    QMap<BCodeEditorDocument *, QString> openingDocuments;
    QMap<BCodeEditorDocument *, QString> savingDocuments;
    QList<BCodeEditorDocument *> closingDocuments;
    QList<BCodeEditorDocument *> processedDocuments;
    QFont editFont;
    BCodeEdit::EditMode editMode;
    int editLineLength;
    BCodeEdit::TabWidth editTabWidth;
    bool bracketsHighlighting;
    QTextCodec *defaultCodec;
    QString defaultFN;
    BAbstractDocumentDriver *driver;
    QMap<QString, BAbstractFileType *> fileTypes;
    BAbstractFileType *defaultFileType;
    BDropHandler *dropHandler;
    QStringList fileHistory;
    int maxHistoryCount;
    QVBoxLayout *vlt;
      QTabWidget *twgt;
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
    friend class BDropHandler;
};

#endif // BCODEEDITOR_P_H
