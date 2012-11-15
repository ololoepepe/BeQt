#ifndef BPLAINTEXTEDIT_H
#define BPLAINTEXTEDIT_H

class BPlainTextEditPrivate;

class QWidget;
class QMimeData;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

class BPlainTextEdit : public QPlainTextEdit, public BBase
{
    B_DECLARE_PRIVATE(BPlainTextEdit)
public:
    explicit BPlainTextEdit(QWidget *parent = 0);
    ~BPlainTextEdit();
    //
    void setDragEnabled(bool b);
    bool dragEnabled() const;
protected:
    BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent = 0);
    //
    bool canInsertFromMimeData(const QMimeData *source) const;
    QMimeData *createMimeDataFromSelection() const;
    void insertFromMimeData(const QMimeData *source);
private:
    Q_DISABLE_COPY(BPlainTextEdit)
};

#endif // BPLAINTEXTEDIT_H

