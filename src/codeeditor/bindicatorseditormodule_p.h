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
================================ BIndicatorsEditorModulePrivate ==============
============================================================================*/

class B_CODEEDITOR_EXPORT BIndicatorsEditorModulePrivate : public BAbstractEditorModulePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BIndicatorsEditorModule)
public:
    struct FileTypeInfo
    {
        QString id;
        QString name;
    };
public:
    explicit BIndicatorsEditorModulePrivate(BIndicatorsEditorModule *q);
    ~BIndicatorsEditorModulePrivate();
public:
    void init();
    void updateCursorPosIndicator();
    void updateEncodingIndicator();
    void updateFileTypeIndicator();
public slots:
    void retranslateUi();
    void cmboxFileTypeCurrentIndexChanged(int index);
public:
    QPointer<QLabel> lblCursorPos;
    QPointer<QLabel> lblEncoding;
    QPointer<QComboBox> cmboxFileType;
private:
    Q_DISABLE_COPY(BIndicatorsEditorModulePrivate)
};

#endif // BINDICATORSEDITORMODULE_P_H
