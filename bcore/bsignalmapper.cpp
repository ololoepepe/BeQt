#include "bsignalmapper.h"

#include <QObject>
#include <QHash>
#include <QVariant>

BSignalMapper::BSignalMapper(QObject *parent) :
    QObject(parent)
{
}

//

void BSignalMapper::setMapping(QObject *sender, const QVariant &data)
{
    _m_varHash.insert(sender, data);
    connect( sender, SIGNAL( destroyed() ), this, SLOT( senderDestroyed() ) );
}

QObject *BSignalMapper::mapping(const QVariant &data) const
{
    return _m_varHash.key(data);
}

void BSignalMapper::removeMappings(QObject *sender)
{
    _m_varHash.remove(sender);
}

//

void BSignalMapper::map()
{
    map( sender() );
}

void BSignalMapper::map(QObject *sender)
{
    if ( _m_varHash.contains(sender) )
        emit mapped( _m_varHash.value(sender) );
}

//

void BSignalMapper::senderDestroyed()
{
    removeMappings( sender() );
}
