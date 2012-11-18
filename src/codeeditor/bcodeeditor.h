#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>

/*============================================================================
================================ Code Editor
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditor : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEditor)
    Q_OBJECT
public:
    explicit BCodeEditor(QWidget *parent = 0);
    ~BCodeEditor();
    //
protected:
    BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEditor)
};

#endif // BCODEEDITOR_H

