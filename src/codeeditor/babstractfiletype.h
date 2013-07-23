#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class BSyntaxHighlighter;
class BTextBlockUserData;
class BAbstractFileTypePrivate;

class QStringList;
class QTextBlock;
class QTextCharFormat;
class QColor;
class QFont;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QString>
#include <QList>

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileType : public BBase
{
    B_DECLARE_PRIVATE(BAbstractFileType)
public:
    struct BracketPair
    {
        QString opening;
        QString closing;
        QString escape;
    };
public:
    typedef QList<BracketPair> BracketPairList;
public:
    explicit BAbstractFileType();
    virtual ~BAbstractFileType();
public:
    static BAbstractFileType *defaultFileType();
    static QString defaultFileTypeId();
    static bool areEqual(const BracketPair &bp1, const BracketPair &bp2);
    static bool areEqual(const BracketPairList &l1, const BracketPairList &l2);
public:
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
    virtual bool matchesFileName(const QString &fileName) const = 0;
    virtual BracketPairList brackets() const = 0;
    QString createFileDialogFilter() const;
protected:
    static BracketPair createBracketPair(const QString &op, const QString &cl, const QString &esc = QString());
protected:
    virtual void highlightBlock(const QString &text) = 0;
    QTextBlock currentBlock() const;
    int currentBlockState() const;
    BTextBlockUserData *currentBlockUserData() const;
    QTextCharFormat format(int position) const;
    int previousBlockState() const;
    void setCurrentBlockState(int newState);
    void setCurrentBlockUserData(BTextBlockUserData *data);
    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
private:
    friend class BSyntaxHighlighter;
};

#endif // BABSTRACTFILETYPE_H
