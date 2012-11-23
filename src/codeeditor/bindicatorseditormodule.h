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
================================ Indicators Editor Module
============================================================================*/

class BIndicatorsEditorModule : public BAbstractEditorModule
{
    B_DECLARE_PRIVATE(BIndicatorsEditorModule)
    Q_OBJECT
public:
    enum Indicator
    {
        CursorPositionIndicator,
        EncodingIndicator,
        FileTypeIndicator
    };
    //
    explicit BIndicatorsEditorModule(QObject *parent = 0);
    ~BIndicatorsEditorModule();
    //
    QString name() const;
    QWidget *createIndicator(Indicator type, QWidget *parent = 0);
    QList<QWidget *> createIndicators(QWidget *parent = 0);
protected:
    BIndicatorsEditorModule(BIndicatorsEditorModulePrivate &d, QObject *parent = 0);
    //
    void editorSet(BCodeEditor *edr);
    void editorUnset(BCodeEditor *edr);
    void documentCursorPositionChanged(const QPoint &pos);
    void documentCodecChanged(const QString &codecName);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void currentDocumentChanged(BCodeEditorDocument *doc);
    void fileTypesChanged();
private:
    Q_DISABLE_COPY(BIndicatorsEditorModule)
};

#endif // BINDICATORSEDITORMODULE_H

