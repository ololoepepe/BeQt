#ifndef BCODEEDIT_H
#define BCODEEDIT_H

class BCodeEditPrivate;

class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QList>

class BCodeEdit : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BCodeEdit)
    Q_OBJECT
public:
    enum TabWidth
    {
        TabWidth2 = 2,
        TabWidth4 = 4,
        TabWidth8 = 8
    };
    //
    struct SplittedLinesRange
    {
        int firstLineNumber;
        int lastLineNumber;
        //
        SplittedLinesRange()
        {
            firstLineNumber = -1;
            lastLineNumber = -1;
        }
    };
    //
    explicit BCodeEdit(QWidget *parent = 0);
    ~BCodeEdit();
    //
    bool isReadOnly() const;
public slots:
    QList<SplittedLinesRange> insertText(const QString &text);
signals:
    void lineSplitted(BCodeEdit::SplittedLinesRange linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
protected:
    BCodeEdit(BCodeEditPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BCodeEdit)
};

#endif // BCODEEDIT_H

