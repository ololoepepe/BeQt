#ifndef BTEXTBLOCKUSERDATA_H
#define BTEXTBLOCKUSERDATA_H

class BTextBlock;

class QString;

#include <BeQtCore/BeQtGlobal>

#include <QTextBlockUserData>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BTextBlockUserData : public QTextBlockUserData
{
public:
    explicit BTextBlockUserData(int sf = -1, int st = -1);
    ~BTextBlockUserData();
public:
    static QString textWithoutComments(const BTextBlockUserData *ud, const QString &text);
    static QString textWithoutComments(const QTextBlock &block);
    static int blockSkipFrom(const QTextBlock &block);
public:
    int skipFrom;
    int skipTo;
};

#endif // BTEXTBLOCKUSERDATA_H
