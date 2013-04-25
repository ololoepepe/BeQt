#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BCodeEditorDocument;
class BAbstractFileType;

class QStringList;
class QTextCodec;
class QMenu;
class QComboBox;

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
    enum CodecGroup
    {
        InvalidGroup = 0,
        UnicodeGroup,
        EastEuropeanGroup,
        WestEuropeanGroup,
        EastAsianGroup,
        SouthEastSouthWestAsianGroup,
        MiddleEastGroup
    };
public:
    explicit BCodeEditor(QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractFileType *> &fileTypes, QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractEditorModule *> &moduleList, QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractFileType *> &fileTypes,
                         const QList<BAbstractEditorModule *> &moduleList, QWidget *parent = 0);
    ~BCodeEditor();
protected:
    explicit BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
public:
    static BAbstractEditorModule *createStandardModule(StandardModule type, BCodeEditor *parent = 0);
    static bool supportsCodec(QTextCodec *codec);
    static bool supportsCodec(const QString &codecName);
    static QList<QTextCodec *> supportedCodecs();
    static QStringList supportedCodecNames();
    static QString codecName(QTextCodec *codec);
    static QString fullCodecName(QTextCodec *codec);
    static QString fullCodecName(const QString &codecName);
    static QList<CodecGroup> codecGroups();
    static QString codecGroupName(CodecGroup group);
    static QList<QTextCodec *> codecsForGroup(CodecGroup group);
    static QStringList codecNamesForGroup(CodecGroup group);
    static QMenu *createPlainCodecsMenu(QObject *receiver, const char *member, QWidget *parent = 0);
    static QMenu *createStructuredCodecsMenu(QObject *receiver, const char *member, QWidget *parent = 0);
    static void retranslateCodecsMenu(QMenu *mnu);
    static QComboBox *createPlainCodecsComboBox(QWidget *parent = 0);
    static QComboBox *createStructuredCodecsComboBox(QWidget *parent = 0);
    static void retranslateCodecsComboBox(QComboBox *cmbox);
    static void selectCodec(QComboBox *cmbox, QTextCodec *codec);
    static void selectCodec(QComboBox *cmbox, const QString &codecName);
    static QTextCodec *selectedCodec(QComboBox *cmbox);
    static QString selectedCodecName(QComboBox *cmbox);
public:
    void setEditFont(const QFont &fnt);
    void setEditFontFamily(const QString &family);
    void setEditFontPointSize(int pointSize);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BCodeEdit::TabWidth tw);
    void setBracketHighlightingEnabled(bool enabled);
    void setDefaultCodec(QTextCodec *codec);
    void setDefaultCodec(const QString &codecName);
    void setDefaultFileName(const QString &fileName);
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
    bool mergeWith(BCodeEditor *other);
    bool waitForAllDocumentsProcessed(int msecs = 30 * BeQt::Second);
    QFont editFont() const;
    QString editFontFamily() const;
    int editFontPointSize() const;
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BCodeEdit::TabWidth editTabWidth() const;
    bool isBracketHighlightingEnabled() const;
    QTextCodec *defaultCodec() const;
    QString defaultCodecName() const;
    QString defaultFileName() const;
    BAbstractEditorModule *module(const QString &name) const;
    BAbstractEditorModule *module(StandardModule type) const;
    QList<BAbstractEditorModule *> modules() const;
    BCodeEditorDocument *currentDocument() const;
    BCodeEditorDocument *document(const QString &fileName) const;
    QList<BCodeEditorDocument *> documents() const;
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
public slots:
    BCodeEditorDocument *addDocument( const QString &fileName = QString() );
    BCodeEditorDocument *addDocument(const QString &fileName, const QString &text);
    QList<BCodeEditorDocument *> openDocuments();
    QList<BCodeEditorDocument *> openDocuments(const QStringList &fileNames, QTextCodec *codec = 0);
    BCodeEditorDocument *openDocument(const QString &fileName, QTextCodec *codec = 0);
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
    virtual BCodeEditorDocument *createDocument(BCodeEditor *editor) const;
signals:
    void defaultCodecChanged(const QString &codecName);
    void editModeChanged(BCodeEdit::EditMode mode);
    void documentAboutToBeAdded(BCodeEditorDocument *doc);
    void documentAdded(BCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BCodeEditorDocument *doc);
    void documentRemoved(const QString &fileName);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void currentDocumentFileNameChanged(const QString &fileName);
    void currentDocumentModificationChanged(bool modified);
    void documentAvailableChanged(bool available);
    void fileTypesChanged();
    void fileHistoryChanged(const QStringList &history);
    void allDocumentsProcessed();
private:
    Q_DISABLE_COPY(BCodeEditor)
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H
