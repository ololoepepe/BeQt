#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>

class BCodeEdit : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEdit)
public:
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
    //
protected:
    BCodeEdit(BCodeEditPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H

