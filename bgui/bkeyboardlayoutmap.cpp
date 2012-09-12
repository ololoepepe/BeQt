#include "bkeyboardlayoutmap.h"

#include <QSharedData>
#include <QSharedPointer>
#include <QMap>
#include <QChar>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QStringList>

BKeyboardLayoutMap::BKeyboardLayoutMap() :
    _m_d(new BKeyboardLayoutMapData)
{
}

BKeyboardLayoutMap::BKeyboardLayoutMap(const BKeyboardLayoutMap &other) :
    _m_d(other._m_d)
{
}

BKeyboardLayoutMap::BKeyboardLayoutMap(const QString &fileName) :
    _m_d(new BKeyboardLayoutMapData)
{
    load(fileName);
}

//

bool BKeyboardLayoutMap::load(const QString &fileName)
{
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return false;
    QTextStream in(&f);
    in.setCodec("UTF-8");
    QString description = in.readLine();
    if ( description.isEmpty() )
    {
        f.close();
        return false;
    }
    QMap<QChar, QChar> directMap;
    QMap<QChar, QChar> reverseMap;
    QList<QChar> directUnique;
    QList<QChar> reverseUnique;
    while ( !in.atEnd() )
    {
        QStringList sl = in.readLine().split(' ', QString::SkipEmptyParts);
        if (sl.size() != 2 || sl.first().length() != 1 || sl.last().length() != 1)
            break;
        const QChar &fc = sl.first().at(0);
        const QChar &sc = sl.last().at(0);
        if ( !directMap.contains(fc) )
        {
            directMap.insert(fc, sc);
            directUnique << fc;
        }
        else
        {
            break;
        }
        if ( !reverseMap.contains(sc) )
        {
            reverseMap.insert(sc, fc);
            reverseUnique << sc;
        }
        else
        {
            break;
        }
    }
    f.close();
    if ( directMap.isEmpty() || reverseMap.isEmpty() )
        return false;
    for (int i = directUnique.size() - 1; i >= 0; --i)
    {
        const QChar &c = directUnique.at(i);
        if ( reverseUnique.contains(c) )
        {
            directUnique.removeAll(c);
            reverseUnique.removeAll(c);
        }
    }
    if ( directUnique.isEmpty() || reverseUnique.isEmpty() )
        return false;
    _m_d->description = description;
    _m_d->directMap = directMap;
    _m_d->reverseMap = reverseMap;
    _m_d->directUnique = directUnique;
    _m_d->reverseUnique = reverseUnique;
    return true;
}

bool BKeyboardLayoutMap::isValid() const
{
    return !_m_d->directMap.isEmpty() && !_m_d->reverseMap.isEmpty() &&
            !_m_d->directUnique.isEmpty() && !_m_d->reverseUnique.isEmpty();
}

bool BKeyboardLayoutMap::switchLayout(const QString &sourceText, QString &newText) const
{
    if ( text.isEmpty() || !isValid() )
        return false;
    const QList<QChar> &du = _m_d->directUnique;
    const QList<QChar> &ru = _m_d->reverseUnique;
    bool direct = false;
    bool reverse = false;
    for (int i = 0; i < du.size(); ++i)
    {
        if ( sourceText.contains( du.at(i) ) )
        {
            direct = true;
            break;
        }
    }
    for (int i = 0; i < ru.size(); ++i)
    {
        if ( sourceText.contains( ru.at(i) ) )
        {
            reverse = true;
            break;
        }
    }
    if ( (direct && reverse) || (!direct && !reverse) )
        return false;
    const QMap<QChar, QChar> &map = direct ? _m_d->directMap : _m_d->reverseMap;
    QList<QChar> keys = map.keys();
    newText = sourceText;
    for (int i = 0; i < newText.length(); ++i)
    {
        const QChar &c = newText.at(i);
        if ( keys.contains(c) )
            newText[i] = map.value(c);
    }
    return true;
}
