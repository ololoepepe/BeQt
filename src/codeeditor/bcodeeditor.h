#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BCodeEditorDocument;

#include <BeQtCore/BeQtGlobal>
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
        BookmarksModule
    };
    //
    static BAbstractEditorModule *createStandardModule(StandardModule type, BCodeEditor *parent = 0);
    //
    explicit BCodeEditor(QWidget *parent = 0);
    ~BCodeEditor();
    //
    void addModule(BAbstractEditorModule *module);
    void addModule(StandardModule type);
    void removeModule(BAbstractEditorModule *module);
    void removeModule(const QString &name);
    void setModules(const QList<BAbstractEditorModule *> &list);
    BAbstractEditorModule *module(const QString &name) const;
    QList<BAbstractEditorModule *> modules() const;
    BCodeEditorDocument *currentDocument() const;
    QList<BCodeEditorDocument *> documents() const;
public slots:
    void addDocument();
    bool openDocument(const QString &fileName);
    bool saveCurrentDocument();
    bool saveCurrentDocumentAs(const QString &newFileName);
    bool closeCurrentDocument();
signals:
    void documentAboutToBeAdded(BCodeEditorDocument *doc);
    void documentAdded(BCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BCodeEditorDocument *doc);
    void currentDocumentChanged(BCodeEditorDocument *doc);
protected:
    BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
    //
    virtual BAbstractDocumentDriver *driverForLoad( BCodeEditorDocument *doc, const QString &newName = QString() );
    virtual BAbstractDocumentDriver *driverForSave(BCodeEditorDocument *doc, QString *newName = 0);
private:
    Q_DISABLE_COPY(BCodeEditor)
    //
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H

