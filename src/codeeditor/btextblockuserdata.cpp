#include "btextblockuserdata.h"

#include <BeQtCore/BeQtGlobal>

#include <QTextBlockUserData>
#include <QString>
#include <QTextBlock>

/*============================================================================
================================ BTextBlockUserData ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextBlockUserData::BTextBlockUserData(int sf, int st)
{
    skipFrom = sf;
    skipTo = st;
}

BTextBlockUserData::~BTextBlockUserData()
{
    //
}

/*============================== Static public methods =====================*/

QString BTextBlockUserData::textWithoutComments(const BTextBlockUserData *ud, const QString &text)
{
    if (!ud || ud->skipFrom < 0)
        return text;
    QString ntext = text;
    int len = ( ud->skipTo >= 0 ? ud->skipTo : text.length() ) - ud->skipFrom;
    ntext.replace( ud->skipFrom, len, QString().fill(' ', len) );
    return ntext;
}

QString BTextBlockUserData::textWithoutComments(const QTextBlock &block)
{
    return textWithoutComments( static_cast<BTextBlockUserData *>( block.userData() ), block.text() );
}

int BTextBlockUserData::blockSkipFrom(const QTextBlock &block)
{
    BTextBlockUserData *ud = static_cast<BTextBlockUserData *>( block.userData() );
    return ud ? ud->skipFrom : -1;
}
