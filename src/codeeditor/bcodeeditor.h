#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;
class BAbstractDocumentDriver;
class BCodeEditorDocument;

#include "bcodeedit.h"

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
    explicit BCodeEditor(const QList<BAbstractEditorModule *> &moduleList, QWidget *parent = 0);
    ~BCodeEditor();
    //
    void setEditFont(const QFont &fnt);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setEditTabWidth(BCodeEdit::TabWidth tw);
    void setBracketHighlightingEnabled(bool enabled);
    void addModule(BAbstractEditorModule *mdl);
    void addModule(StandardModule type);
    void removeModule(BAbstractEditorModule *mdl);
    void removeModule(const QString &name);
    void setModules(const QList<BAbstractEditorModule *> &list);
    void setDriver(BAbstractDocumentDriver *drv);
    QFont editFont() const;
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BCodeEdit::TabWidth editTabWidth() const;
    bool isBracketHighlightingEnabled() const;
    BAbstractEditorModule *module(const QString &name) const;
    QList<BAbstractEditorModule *> modules() const;
    BCodeEditorDocument *currentDocument() const;
    QList<BCodeEditorDocument *> documents() const;
    BAbstractDocumentDriver *driver() const;
public slots:
    void addDocument(const QString &fileName);
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
private:
    Q_DISABLE_COPY(BCodeEditor)
    //
    friend class BAbstractEditorModulePrivate;
};

#endif // BCODEEDITOR_H

