#ifndef BSETTINGSNODE_H
#define BSETTINGSNODE_H

class BSettingsNodePrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QList>
#include <QVariant>
#include <QChar>
#include <QCoreApplication>

/*============================================================================
================================ BSettingsNode ===============================
============================================================================*/

class B_CORE_EXPORT BSettingsNode : public BBase
{
    Q_DECLARE_TR_FUNCTIONS(BSettingsNode)
    B_DECLARE_PRIVATE(BSettingsNode)
public:
    typedef bool (*SetFunction)(QVariant &);
    typedef bool (*ShowFunction)(const QVariant &);
public:
    explicit BSettingsNode(BSettingsNode *parent = 0);
    explicit BSettingsNode(const QString &key, BSettingsNode *parent = 0);
    explicit BSettingsNode(QVariant::Type type, const QString &key, BSettingsNode *parent = 0);
    ~BSettingsNode();
public:
    void setParent(BSettingsNode *parent);
    void setKey(const QString &key);
    void setType(QVariant::Type type);
    void setDescription(const QString &s);
    void setUserSetFunction(SetFunction f);
    void setUserShowFunction(ShowFunction f);
    void addChild(BSettingsNode *node);
    void addChild(const QString &key, QVariant::Type type = QVariant::String);
    void removeChild(BSettingsNode *node);
    void showTree(int indentStep = 2, int initialIndent = 0) const;
    BSettingsNode *find(const QString &path, QChar separator = '.');
    const BSettingsNode *find(const QString &path, QChar separator = '.') const;
    bool set(QString path, QChar separator = '.') const;
    bool set(QString path, QString text, QChar separator = '.') const; //%k - key
    bool set(QString path, QVariant value, QChar separator = '.') const;
    bool show(QString path, QChar separator = '.') const;
    bool show(QString path, QString text, QChar separator = '.') const; //%k - key, %v - value
    BSettingsNode *clone(BSettingsNode *parent = 0) const;
    BSettingsNode *parent() const;
    QList<BSettingsNode *> childNodes() const;
    int childCount() const;
    QString key() const;
    QVariant::Type type() const;
    QString description() const;
    SetFunction userSetFunction() const;
    ShowFunction userShowFunction() const;
private:
    Q_DISABLE_COPY(BSettingsNode)
};

#endif // BSETTINGSNODE_H