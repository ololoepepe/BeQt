/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "btextblockuserdata.h"

#include <BeQtCore/BeQtGlobal>

#include <QTextBlockUserData>
#include <QString>
#include <QTextBlock>

#include <QDebug>

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
