#ifndef BSIGNALMAPPER_H
#define BSIGNALMAPPER_H

#include <QObject>
#include <QHash>
#include <QVariant>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BSignalMapper : public QObject
{
    Q_OBJECT
public:
    explicit BSignalMapper(QObject *parent = 0);
    //
    void setMapping(QObject *sender, const QVariant &data);
    QObject *mapping(const QVariant &data) const;
    void removeMappings(QObject *sender);
public slots:
    void map();
    void map(QObject *sender);
private:
    QHash<QObject *, QVariant> _m_varHash;
private slots:
    void senderDestroyed();
signals:
    void mapped(const QVariant &data);
};

#endif // BSIGNALMAPPER_H
