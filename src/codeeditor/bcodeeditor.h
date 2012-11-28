#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BCodeEditorDocument;
class BAbstractFileType;

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
    enum CodecGroup
    {
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
    static QString codecGroupName(CodecGroup group);
    static QList<QTextCodec *> codecsForGroup(CodecGroup group);
    static QStringList codecNamesForGroup(CodecGroup group);
public:
    void setEditFont(const QFont &fnt);
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
    void setFileHistory(const QStringList &list);
    void setMaxHistoryCount(int count);
    bool waitForAllDocumentsProcessed(int msecs = 30 * BeQt::Second);
    QFont editFont() const;
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
    QList<BCodeEditorDocument *> documents() const;
    BAbstractDocumentDriver *driver() const;
    BAbstractFileType *fileType(const QString &id) const;
    QList<BAbstractFileType *> fileTypes() const;
    QStringList fileHistory() const;
    int maxHistoryCount() const;
    bool documentAvailable() const;
    QString currentFileName() const;
    QStringList fileNames() const;
    QObject *dropHandler() const;
public slots:
    bool addDocument( const QString &fileName = QString() );
    bool addDocument(const QString &fileName, const QString &text);
    bool openDocuments();
    bool openDocuments(const QStringList &fileNames, QTextCodec *codec = 0);
    bool openDocument(const QString &fileName, QTextCodec *codec = 0);
    bool reopenCurrentDocument(QTextCodec *codec = 0);
    bool saveCurrentDocument();
    bool saveCurrentDocumentAs();
    bool saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec = 0);
    bool saveAllDocuments();
    bool closeCurrentDocument();
    bool closeAllDocuments();
signals:
    void defaultCodecChanged(const QString &codecName);
    void editModeChanged(BCodeEdit::EditMode mode);
    void documentAboutToBeAdded(BCodeEditorDocument *doc);
    void documentAdded(BCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BCodeEditorDocument *doc);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void documentAvailableChanged(bool available);
    void fileTypesChanged();
    void fileHistoryChanged(const QStringList &history);
    void allDocumentsProcessed();
private:
    Q_DISABLE_COPY(BCodeEditor)
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H

