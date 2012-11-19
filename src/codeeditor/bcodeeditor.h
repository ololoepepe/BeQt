#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;
class BAbstractEditorModule;

class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>

/*============================================================================
================================ Code Editor
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditor : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEditor)
    Q_OBJECT
public:
    enum EditorFeature
    {
        NoFeature = 0
    };
    //
    explicit BCodeEditor(QWidget *parent = 0);
    ~BCodeEditor();
    //
    void addModule(BAbstractEditorModule *module);
    void setModules(const QList<BAbstractEditorModule *> &list);
    BAbstractEditorModule *module(const QString &name) const;
    BAbstractEditorModule *moduleForFeature(EditorFeature feature) const;
    QList<BAbstractEditorModule *> modules() const;
protected:
    BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditor)
};

#endif // BCODEEDITOR_H

