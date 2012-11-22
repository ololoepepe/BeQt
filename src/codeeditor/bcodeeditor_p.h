#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BCodeEditorDocument;
class BAbstractEditorModule;
class BAbstractDocumentDriver;

class QVBoxLayout;
class QTabWidget;
class QString;
class QPoint;
class QFont;

#include "bcodeeditor.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QString>
#include <QList>

/*============================================================================
================================ Code Editor Private
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEditor)
    B_DECLARE_TR_FUNCTIONS(BCodeEditor, q)
    Q_OBJECT
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
    //
    void init();
    bool acceptFileName(const QString &fileName, bool *readOnly) const;
    QString defaultFileName() const;
    BCodeEditorDocument *createDocument( const QString &fileName = QString(), const QString &text = QString() );
    void addDocument(BCodeEditorDocument *doc);
    bool closeDocument(BCodeEditorDocument *doc);
    void updateDocumentTab(BCodeEditorDocument *doc);
    //Messages
    void failedToOpenMessage(const QString &fileName);
    void failedToSaveMessage(const QString &fileName);
    //Signal emitting
    void emitDocumentAboutToBeAdded(BCodeEditorDocument *doc);
    void emitDocumentAdded(BCodeEditorDocument *doc);
    void emitDocumentAboutToBeRemoved(BCodeEditorDocument *doc);
    void emitCurrentDocumentChanged(BCodeEditorDocument *doc);
    //External private class call
    void setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr);
    //
    QMap<QString, BAbstractEditorModule *> modules;
    BCodeEditorDocument *document;
    QMap<BCodeEditorDocument *, QString> openingDocuments;
    QFont editFont;
    BCodeEdit::EditMode editMode;
    int editLineLength;
    BCodeEdit::TabWidth editTabWidth;
    bool bracketsHighlighting;
    BCodeEditor::DuplicateAcceptMode acceptMode;
    BAbstractDocumentDriver *driver;
    //
    QVBoxLayout *vlt;
      QTabWidget *twgt;
public slots:
    void twgtCurrentChanged(int index);
    void twgtTabCloseRequested(int index);
    //BCodeEdit events
    void updateDocumentReadOnly(bool ro);
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
    void documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
    //BCodeEditorDocument events
    void documentFileNameChanged(const QString &fn);
    void documentCodecChanged(const QString &codecName);
    void documentLoadingFinished(bool success);
    void documentSavingFinished(bool success);
    void documentBuisyChanged(bool buisy);
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
};

#endif // BCODEEDITOR_P_H

