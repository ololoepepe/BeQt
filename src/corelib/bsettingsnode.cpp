#include "bsettingsnode.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bterminaliohandler.h"
#include "bcoreapplication.h"

#include <QString>
#include <QList>
#include <QVariant>
#include <QChar>
#include <QStringList>
#include <QSettings>
#include <QLocale>

#include <QDebug>

/*============================================================================
================================ BSettingsNodePrivate ========================
============================================================================*/

class BSettingsNodePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSettingsNode)
public:
    explicit BSettingsNodePrivate(BSettingsNode *q);
    ~BSettingsNodePrivate();
public:
    void init();
public:
    BSettingsNode *parentNode;
    QList<BSettingsNode *> childNodes;
    QString key;
    QVariant::Type type;
    QString description;
    BSettingsNode::SetFunction setFunction;
    BSettingsNode::ShowFunction showFunction;
private:
    Q_DISABLE_COPY(BSettingsNodePrivate)
};

/*============================================================================
================================ BSettingsNodePrivate ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsNodePrivate::BSettingsNodePrivate(BSettingsNode *q) :
    BBasePrivate(q)
{
    //
}

BSettingsNodePrivate::~BSettingsNodePrivate()
{
    q_func()->setParent(0);
    foreach (BSettingsNode *n, childNodes)
        delete n;
}

/*============================== Public methods ============================*/

void BSettingsNodePrivate::init()
{
    parentNode = 0;
    type = QVariant::String;
    setFunction = 0;
    showFunction = 0;
}

/*============================================================================
================================ BSettingsNode ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsNode::BSettingsNode(BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
}

BSettingsNode::BSettingsNode(const QString &key, BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
    setKey(key);
}

BSettingsNode::BSettingsNode(QVariant::Type type, const QString &key, BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
    setType(type);
    setKey(key);
}

BSettingsNode::~BSettingsNode()
{
    //
}

/*============================== Public methods ============================*/

void BSettingsNode::setParent(BSettingsNode *parent)
{
    if (parent == d_func()->parentNode)
        return;
    if (d_func()->parentNode)
        d_func()->parentNode->d_func()->childNodes.removeAll(this);
    d_func()->parentNode = parent;
    if (parent && !parent->d_func()->childNodes.contains(this))
        parent->d_func()->childNodes << this;
}

void BSettingsNode::setKey(const QString &key)
{
    d_func()->key = key;
}

void BSettingsNode::setType(QVariant::Type type)
{
    d_func()->type = type;
}

void BSettingsNode::setDescription(const QString &s)
{
    d_func()->description = s;
}

void BSettingsNode::setUserSetFunction(SetFunction f)
{
    d_func()->setFunction = f;
}

void BSettingsNode::setUserShowFunction(ShowFunction f)
{
    d_func()->showFunction = f;
}

void BSettingsNode::addChild(BSettingsNode *node)
{
    if (!node || d_func()->childNodes.contains(node))
        return;
    node->setParent(this);
}

void BSettingsNode::addChild(const QString &key, QVariant::Type type)
{
    addChild(new BSettingsNode(type, key));
}

void BSettingsNode::removeChild(BSettingsNode *node)
{
    if (!node)
        return;
    node->setParent(0);
}

void BSettingsNode::showTree(int indentStep, int initialIndent) const
{
    if (indentStep < 0)
        indentStep = 2;
    if (initialIndent < 0)
        initialIndent = 0;
    if (!key().isEmpty())
        BTerminalIOHandler::writeLine(QString(initialIndent, ' ') + key());
    foreach (BSettingsNode *n, childNodes())
        n->showTree(indentStep, initialIndent + (!key().isEmpty() ? indentStep : 0));
}

BSettingsNode *BSettingsNode::find(const QString &path, QChar separator)
{
    if (path.isEmpty())
        return 0;
    if (separator.isNull())
        separator = '.';
    if (key().isEmpty())
    {
        foreach (BSettingsNode *n, childNodes())
        {
            BSettingsNode *nn = n->find(path, separator);
            if (nn)
                return nn;
        }
    }
    else
    {
        QStringList sl = path.split(separator);
        if (sl.takeFirst() != key())
            return 0;
        QString spath = sl.join(QString(separator));
        if (spath.isEmpty())
            return this;
        foreach (BSettingsNode *n, childNodes())
        {
            BSettingsNode *nn = n->find(spath, separator);
            if (nn)
                return nn;
        }
    }
    return 0;
}

const BSettingsNode *BSettingsNode::find(const QString &path, QChar separator) const
{
    return const_cast<BSettingsNode *>(this)->find(path, separator);
}

bool BSettingsNode::set(QString path, QChar separator) const
{
    return set(path, tr("Enter value for") + " \"%k\": ", separator);
}

bool BSettingsNode::set(QString path, QString text, QChar separator) const
{
    const BSettingsNode *n = find(path, separator);
    if (!n)
        return false;
    QVariant v;
    if (n->userSetFunction() && !n->userSetFunction()(v))
        return false;
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    v = BTerminalIOHandler::readLine(text.replace("%k", path.split("/").last()));
    switch (n->type())
    {
    case QVariant::Locale:
        v = QLocale(v.toString());
        break;
    default:
        if (!v.convert(n->type()))
            return false;
        break;
    }
    bSettings->setValue(path, v);
    return true;
}

bool BSettingsNode::set(QString path, QVariant value, QChar separator) const
{
    const BSettingsNode *n = find(path, separator);
    if (!n)
        return false;
    QVariant v = value;
    if (n->userSetFunction() && !n->userSetFunction()(v))
        return false;
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    bSettings->setValue(path, v);
    return true;
}

bool BSettingsNode::show(QString path, QChar separator) const
{
    return show(path, tr("The value for") + " \"%k\": %v", separator);
}

bool BSettingsNode::show(QString path, QString text, QChar separator) const
{
    const BSettingsNode *n = find(path, separator);
    if (!n)
        return false;
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    QVariant v = bSettings->value(path);
    if (n->userShowFunction() && !n->userShowFunction()(v))
        return false;
    QString vs = v.toString();
    switch (n->type())
    {
    case QVariant::Locale:
        vs = v.toLocale().name();
        break;
    default:
        break;
    }
    if (vs.isEmpty())
        vs = "[]";
    BTerminalIOHandler::writeLine(text.replace("%k", path.split("/").last()).replace("%v", vs));
    return true;
}

BSettingsNode *BSettingsNode::clone(BSettingsNode *parent) const
{
    BSettingsNode *root = new BSettingsNode(parent);
    root->setKey(key());
    root->setType(type());
    root->setDescription(description());
    root->setUserSetFunction(userSetFunction());
    root->setUserShowFunction(userShowFunction());
    foreach (BSettingsNode *n, d_func()->childNodes)
        root->addChild(n->clone(root));
    return root;
}

BSettingsNode *BSettingsNode::parent() const
{
    return d_func()->parentNode;
}

QList<BSettingsNode *> BSettingsNode::childNodes() const
{
    return d_func()->childNodes;
}

int BSettingsNode::childCount() const
{
    return d_func()->childNodes.size();
}

QString BSettingsNode::key() const
{
    return d_func()->key;
}

QVariant::Type BSettingsNode::type() const
{
    return d_func()->type;
}

QString BSettingsNode::description() const
{
    return d_func()->description;
}

BSettingsNode::SetFunction BSettingsNode::userSetFunction() const
{
    return d_func()->setFunction;
}

BSettingsNode::ShowFunction BSettingsNode::userShowFunction() const
{
    return d_func()->showFunction;
}