#ifndef BINDICATORSEDITORMODULE_P_H
#define BINDICATORSEDITORMODULE_P_H

class QLabel;
class QWidget;
class QPoint;
class QString;
class QComboBox;

#include "bindicatorseditormodule.h"
#include "babstracteditormodule_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>
#include <QList>
#include <QPointer>

/*============================================================================
================================ Indicators Editor Module Private
============================================================================*/

class B_CODEEDITOR_EXPORT BIndicatorsEditorModulePrivate : public BAbstractEditorModulePrivate
{
    B_DECLARE_PUBLIC(BIndicatorsEditorModule)
    B_DECLARE_TR_FUNCTIONS(BIndicatorsEditorModule, q)
    Q_OBJECT
public:
    struct FileTypeInfo
    {
        QString id;
        QString name;
    };
    //
    explicit BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q);
    ~BIndicatorsEditorModulePrivate();
    //
    void init();
    void updateCursorPosIndicator();
    void updateEncodingIndicator();
    void updateFileTypeIndicator();
    //
    QPointer<QLabel> lblCursorPos;
    QPointer<QLabel> lblEncoding;
    QPointer<QComboBox> cmboxFileType;
public slots:
    void retranslateUi();
    void cmboxFileTypeCurrentIndexChanged(int index);
private:
    Q_DISABLE_COPY(BIndicatorsEditorModulePrivate)
};

#endif // BINDICATORSEDITORMODULE_P_H

