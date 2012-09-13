#ifndef BKEYBOARDLAYOUTMAP_H
#define BKEYBOARDLAYOUTMAP_H

#include <QSharedData>
#include <QSharedPointer>
#include <QMap>
#include <QChar>
#include <QList>

class BKeyboardLayoutMapData : public QSharedData
{
public:
    QString description;
    QMap<QChar, QChar> directMap;
    QMap<QChar, QChar> reverseMap;
    QList<QChar> directUnique;
    QList<QChar> reverseUnique;
};

class BKeyboardLayoutMap
{
public:
    BKeyboardLayoutMap();
    BKeyboardLayoutMap(const BKeyboardLayoutMap &other);
    explicit BKeyboardLayoutMap(const QString &fileName);
    //
    bool load(const QString &fileName);
    bool isValid() const;
    const QString &description() const;
    bool switchLayout(QString &text) const;
private:
    QSharedPointer<BKeyboardLayoutMapData> _m_d;
};

#endif // BKEYBOARDLAYOUTMAP_H
