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

/*============================================================================
================================ Indicators Editor Module Private
============================================================================*/

class BIndicatorsEditorModulePrivate : public BAbstractEditorModulePrivate
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
    QLabel *createCursorPosIndicator(QWidget *parent = 0);
    QLabel *createEncodingIndicator(QWidget *parent = 0);
    QComboBox *createFileTypeIndicator(QWidget *parent = 0);
    QString createCursorPosIndicatorText() const;
    QString createEncodingIndicatorText() const;
    QList<FileTypeInfo> createFileTypeInfos() const;
    int createFileTypeIndicatorIndex() const;
    void processFileTypeIndicator(QComboBox *cmbox, const QList<FileTypeInfo> &infos, int index);
    void updateCursorPosIndicators();
    void updateEncodingIndicators();
    void updateFileTypeIndicators();
    //
    QMap<QObject *, QLabel *> cursorPosIndicators;
    QMap<QObject *, QLabel *> encodingIndicators;
    QMap<QObject *, QComboBox *> fileTypeIndicators;
public slots:
    void retranslateUi();
    void cursorPosIndicatorDestroyed(QObject *obj);
    void encodingIndicatorDestroyed(QObject *obj);
    void fileTypeIndicatorDestroyed(QObject *obj);
    void cmboxCurrentIndexChanged(int index);
private:
    Q_DISABLE_COPY(BIndicatorsEditorModulePrivate)
};

#endif // BINDICATORSEDITORMODULE_P_H

