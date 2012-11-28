#ifndef BINDICATORSEDITORMODULE_H
#define BINDICATORSEDITORMODULE_H

class BIndicatorsEditorModulePrivate;
class BCodeEditor;
class BCodeEditorDocument;
class BAbstractFileType;

class QString;
class QWidget;

#include "babstracteditormodule.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QList>

/*============================================================================
================================ BIndicatorsEditorModule =====================
============================================================================*/

class B_CODEEDITOR_EXPORT BIndicatorsEditorModule : public BAbstractEditorModule
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BIndicatorsEditorModule)
public:
    enum Indicator
    {
        CursorPositionIndicator,
        EncodingIndicator,
        FileTypeIndicator
    };
public:
    explicit BIndicatorsEditorModule(QObject *parent = 0);
    ~BIndicatorsEditorModule();
protected:
    explicit BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent = 0);
public:
    QString id() const;
    QWidget *indicator(Indicator type);
    QList<QWidget *> indicators();
protected:
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentCursorPositionChanged(const QPoint &pos);
    void documentCodecChanged(const QString &codecName);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void defaultCodecChanged(const QString &codecName);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void fileTypesChanged();
private:
    Q_DISABLE_COPY(BIndicatorsEditorModule)
};

#endif // BINDICATORSEDITORMODULE_H

