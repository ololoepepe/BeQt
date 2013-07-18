#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BAbstractFileType;
class BAbstractEditorModulePrivate;
class BAbstractCodeEditorDocument;
class BSpellChecker;

class QStringList;
class QTextCodec;

#include "bcodeedit.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>
#include <QString>

/*============================================================================
================================ BCodeEditor =================================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditor : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEditor)
public:
    enum StandardModule
    {
        BookmarksModule,
        EditModule,
        IndicatorsModule,
        OpenSaveModule,
        SearchModule
    };
    enum StandardDocumentType
    {
        StandardDocument,
        SimpleDocument
    };
public:
    typedef QList<BAbstractCodeEditorDocument *> DocumentList;
public:
    explicit BCodeEditor(QWidget *parent = 0);
    explicit BCodeEditor(StandardDocumentType t, QWidget *parent = 0);
    ~BCodeEditor();
protected:
    explicit BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
public:
    static BAbstractEditorModule *createStandardModule(StandardModule type, BCodeEditor *parent = 0);
public:
    void setDocumentType(StandardDocumentType t);
    void setEditFont(const QFont &fnt);
    void setEditFontFamily(const QString &family);
    void setEditFontPointSize(int pointSize);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setLineNumberWidgetVisible(bool b);
    void setBracketHighlightingEnabled(bool b);
    void setSpellChecker(BSpellChecker *sc);
    void setDefaultCodec(QTextCodec *codec);
    void setDefaultCodec(const QString &codecName);
    void setDefaultFileName(const QString &fileName);
    void setMaximumFileSize(int sz);
    void setAsyncProcessingMinimumLength(int length);
    void addModule(BAbstractEditorModule *mdl);
    void addModule(StandardModule type);
    void removeModule(BAbstractEditorModule *mdl);
    void removeModule(const QString &name);
    void setModules(const QList<BAbstractEditorModule *> &list);
    void setDriver(BAbstractDocumentDriver *drv);
    void addFileType(BAbstractFileType *ft);
    void removeFileType(BAbstractFileType *ft);
    void removeFileType(const QString &id);
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void setPreferredFileType(BAbstractFileType *ft);
    void setPreferredFileType(const QString &id);
    void setFileHistory(const QStringList &list);
    void setMaxHistoryCount(int count);
    void mergeWith(BCodeEditor *other);
    bool isBuisy() const;
    bool waitForAllDocumentsProcessed(int msecs = 30 * BeQt::Second) const;
    StandardDocumentType documentType() const;
    QFont editFont() const;
    QString editFontFamily() const;
    int editFontPointSize() const;
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BeQt::TabWidth editTabWidth() const;
    bool lineNumberWidgetVisible() const;
    bool isBracketHighlightingEnabled() const;
    BSpellChecker *spellChecker() const;
    QTextCodec *defaultCodec() const;
    QString defaultCodecName() const;
    QString defaultFileName() const;
    int maximumFileSize() const;
    int asyncProcessingMinimumLength() const;
    BAbstractEditorModule *module(const QString &name) const;
    BAbstractEditorModule *module(StandardModule type) const;
    QList<BAbstractEditorModule *> modules() const;
    BAbstractCodeEditorDocument *currentDocument() const;
    BAbstractCodeEditorDocument *document(const QString &fileName) const;
    DocumentList documents() const;
    BAbstractDocumentDriver *driver() const;
    BAbstractFileType *fileType(const QString &id) const;
    QList<BAbstractFileType *> fileTypes() const;
    BAbstractFileType *preferredFileType() const;
    QString preferredFileTypeId() const;
    QStringList fileHistory() const;
    int maxHistoryCount() const;
    bool documentAvailable() const;
    QString currentDocumentFileName() const;
    bool isCurrentDocumentModified() const;
    QStringList fileNames() const;
    QObject *dropHandler() const;
    QObject *closeHandler() const;
public Q_SLOTS:
    BAbstractCodeEditorDocument *addDocument(const QString &fileName = QString());
    BAbstractCodeEditorDocument *addDocument(const QString &fileName, const QString &text);
    DocumentList openDocuments();
    DocumentList openDocuments(const QStringList &fileNames, QTextCodec *codec = 0);
    BAbstractCodeEditorDocument *openDocument(const QString &fileName, QTextCodec *codec = 0);
    bool reopenCurrentDocument(QTextCodec *codec = 0);
    bool reopenCurrentDocument(const QString &codecName);
    bool saveCurrentDocument();
    bool saveCurrentDocumentAs();
    bool saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec = 0);
    bool saveAllDocuments();
    bool closeCurrentDocument();
    bool closeAllDocuments(bool save = true);
    void insertTextIntoCurrentDocument(const QString &text);
    void setCurrentDocumentText(const QString &text);
protected:
    virtual BAbstractCodeEditorDocument *createDocument(BCodeEditor *editor) const;
    virtual void showClosingMessage(QWidget *parent);
Q_SIGNALS:
    void defaultCodecChanged(const QString &codecName);
    void editModeChanged(BCodeEdit::EditMode mode);
    void documentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    void documentAdded(BAbstractCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    void documentRemoved(const QString &fileName);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void currentDocumentFileNameChanged(const QString &fileName);
    void currentDocumentModificationChanged(bool modified);
    void documentAvailableChanged(bool available);
    void fileTypesChanged();
    void fileHistoryChanged(const QStringList &history);
    bool buisyChanged(bool buisy);
    void allDocumentsProcessed();
private:
    Q_DISABLE_COPY(BCodeEditor)
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H
