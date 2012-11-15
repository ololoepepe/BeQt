#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;
class QPaintEvent;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

class B_WIDGETS_EXPORT BPlainTextEdit : public QPlainTextEdit, public BBase
{
    B_DECLARE_PRIVATE(BPlainTextEdit)
    Q_OBJECT
public:
    enum Mode
    {
        NormalMode,
        BlockMode
    };
    //
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
    //
    void setDragEnabled(bool b);
    void setMode(Mode mode);
    void setLineLength(int length);
    bool dragEnabled() const;
    Mode mode() const;
    int lineLength() const;
protected:
    BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
    //
    bool canInsertFromMimeData(const QMimeData *source) const;
    QMimeData *createMimeDataFromSelection() const;
    void insertFromMimeData(const QMimeData *source);
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

