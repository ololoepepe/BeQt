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
================================ Code Editor
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditor : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEditor)
    Q_OBJECT
public:
    enum StandardModule
    {
        BookmarksModule,
        IndicatorsModule,
        SearchModule
    };
    enum EncodingGroup
    {
        UnicodeGroup,
        EastEuropeanGroup,
        WestEuropeanGroup,
        EastAsianGroup,
        SouthEastSouthWestAsianGroup,
        MiddleEastGroup
    };
    //
    static BAbstractEditorModule *createStandardModule(StandardModule type, BCodeEditor *parent = 0);
    //
    explicit BCodeEditor(QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractFileType *> &fileTypes, QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractEditorModule *> &moduleList, QWidget *parent = 0);
    explicit BCodeEditor(const QList<BAbstractFileType *> &fileTypes,
                         const QList<BAbstractEditorModule *> &moduleList, QWidget *parent = 0);
    ~BCodeEditor();
    //
    void setEditFont(const QFont &fnt);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BCodeEdit::TabWidth tw);
    void setBracketHighlightingEnabled(bool enabled);
    void setDefaultCodec(QTextCodec *codec);
    void setDefaultCodec(const QString &codecName);
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
    bool waitForAllDocumentsProcessed(int msecs = 30 * BeQt::Second);
    QFont editFont() const;
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BCodeEdit::TabWidth editTabWidth() const;
    bool isBracketHighlightingEnabled() const;
    QTextCodec *defaultCodec() const;
    QString defaultCodecName() const;
    BAbstractEditorModule *module(const QString &name) const;
    BAbstractEditorModule *module(StandardModule type) const;
    QList<BAbstractEditorModule *> modules() const;
    BCodeEditorDocument *currentDocument() const;
    QList<BCodeEditorDocument *> documents() const;
    BAbstractDocumentDriver *driver() const;
    BAbstractFileType *fileType(const QString &id) const;
    QList<BAbstractFileType *> fileTypes() const;
    bool documentAvailable() const;
    QString currentFileName() const;
    QStringList fileNames() const;
    bool supportsCodec(QTextCodec *codec) const;
    bool supportsCodec(const QString &codecName) const;
    QString fullCodecName(QTextCodec *codec) const;
    QString fullCodecName(const QString &codecName) const;
    QString encodingGroupName(EncodingGroup group) const;
    QList<QTextCodec *> codecsForGroup(EncodingGroup group) const;
    QStringList codecNamesForGroup(EncodingGroup group) const;
public slots:
    bool addDocument( const QString &fileName = QString() );
    bool addDocument(const QString &fileName, const QString &text);
    bool openDocument(const QString &fileName, QTextCodec *codec = 0);
    bool saveCurrentDocument();
    bool saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec = 0);
    bool saveAllDocuments();
    bool closeCurrentDocument();
    bool closeAllDocuments();
signals:
    void documentAboutToBeAdded(BCodeEditorDocument *doc);
    void documentAdded(BCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BCodeEditorDocument *doc);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void documentAvailableChanged(bool available);
    void fileTypesChanged();
    void allDocumentsProcessed();
protected:
    BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditor)
    //
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H

