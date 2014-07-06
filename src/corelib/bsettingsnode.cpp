/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "bsettingsnode.h"

#include "bapplicationbase.h"
#include "bbase_p.h"
#include "bterminal.h"
#include "btranslation.h"

#include <QChar>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

/*============================================================================
================================ BSettingsNodePrivate ========================
============================================================================*/

class BSettingsNodePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSettingsNode)
public:
    QList<BSettingsNode *> childNodes;
    BTranslation description;
    QString key;
    BSettingsNode *parentNode;
    bool secure;
    BSettingsNode::SetFunction setFunction;
    BSettingsNode::ShowFunction showFunction;
    QVariant::Type type;
public:
    explicit BSettingsNodePrivate(BSettingsNode *q);
    ~BSettingsNodePrivate();
public:
    void init();
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
    secure = false;
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

/*============================== Static public methods =====================*/

QVariant BSettingsNode::stringToVariant(const QString &s, QVariant::Type t, bool *ok)
{
    if (ok)
        *ok = true;
    QVariant v;
    switch (t) {
    case QVariant::Locale:
        v = QLocale(s);
        break;
    default:
        v = s;
        if (!v.convert(t) && ok)
            *ok = false;
        break;
    }
    return v;
}

QString BSettingsNode::variantToString(QVariant v, bool *ok)
{
    if (ok)
        *ok = true;
    QString s;
    switch (v.type()) {
    case QVariant::Locale:
        s = v.toLocale().name();
        break;
    default:
        if (!v.convert(QVariant::String) && ok)
            *ok = false;
        else
            s = v.toString();
        break;
    }
    return s;
}

/*============================== Public methods ============================*/

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

int BSettingsNode::childCount() const
{
    return d_func()->childNodes.size();
}

QList<BSettingsNode *> BSettingsNode::childNodes() const
{
    return d_func()->childNodes;
}

BSettingsNode *BSettingsNode::clone(BSettingsNode *parent) const
{
    BSettingsNode *root = new BSettingsNode(parent);
    root->setKey(key());
    root->setType(type());
    root->setSecureInput(secureInput());
    root->setDescription(description());
    root->setUserSetFunction(userSetFunction());
    root->setUserShowFunction(userShowFunction());
    foreach (BSettingsNode *n, d_func()->childNodes)
        root->addChild(n->clone(root));
    return root;
}

BTranslation BSettingsNode::description() const
{
    return d_func()->description;
}

BSettingsNode *BSettingsNode::find(const QString &path, QChar separator)
{
    if (path.isEmpty())
        return 0;
    if (separator.isNull())
        separator = '.';
    if (key().isEmpty()) {
        foreach (BSettingsNode *n, childNodes()) {
            BSettingsNode *nn = n->find(path, separator);
            if (nn)
                return nn;
        }
    } else {
        QStringList sl = path.split(separator);
        if (sl.takeFirst() != key())
            return 0;
        QString spath = sl.join(QString(separator));
        if (spath.isEmpty())
            return this;
        foreach (BSettingsNode *n, childNodes()) {
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

QString BSettingsNode::key() const
{
    return d_func()->key;
}

BSettingsNode *BSettingsNode::parent() const
{
    return d_func()->parentNode;
}

void BSettingsNode::removeChild(BSettingsNode *node)
{
    if (!node)
        return;
    node->setParent(0);
}

bool BSettingsNode::secureInput() const
{
    return d_func()->secure;
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
    if (n->userSetFunction())
        return n->userSetFunction()(n, v);
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    QString t = text.replace("%k", path.split("/").last());
    QString s = n->secureInput() ? bReadLineSecure(t) : bReadLine(t);
    bool ok = false;
    v = stringToVariant(s, n->type(), &ok);
    if (!ok)
        return false;
    if (path == "BeQt/Core/locale")
        BApplicationBase::setLocale(v.toLocale());
    else
        bSettings->setValue(path, v);
    return true;
}

bool BSettingsNode::set(QString path, QVariant value, QChar separator) const
{
    const BSettingsNode *n = find(path, separator);
    if (!n)
        return false;
    QVariant v = value;
    if (n->userSetFunction())
        return n->userSetFunction()(n, v);
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    if (path == "BeQt/Core/locale")
        BApplicationBase::setLocale(v.toLocale());
    else
        bSettings->setValue(path, v);
    return true;
}

void BSettingsNode::setDescription(const BTranslation &t)
{
    d_func()->description = t;
}

void BSettingsNode::setKey(const QString &key)
{
    d_func()->key = key;
}

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

void BSettingsNode::setSecureInput(bool b)
{
    d_func()->secure = b;
}

void BSettingsNode::setType(QVariant::Type type)
{
    d_func()->type = type;
}

void BSettingsNode::setUserSetFunction(SetFunction f)
{
    d_func()->setFunction = f;
}

void BSettingsNode::setUserShowFunction(ShowFunction f)
{
    d_func()->showFunction = f;
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
    if (n->userShowFunction())
        return n->userShowFunction()(n, v);
    bool ok = false;
    QString vs = variantToString(v, &ok);
    if (!ok)
        return false;
    BTerminal::writeLine(text.replace("%k", path.split("/").last()).replace("%v", vs));
    return true;
}

void BSettingsNode::showTree(int indentStep, int initialIndent) const
{
    if (indentStep < 0)
        indentStep = 2;
    if (initialIndent < 0)
        initialIndent = 0;
    if (!key().isEmpty())
        BTerminal::writeLine(QString(initialIndent, ' ') + key());
    foreach (BSettingsNode *n, childNodes())
        n->showTree(indentStep, initialIndent + (!key().isEmpty() ? indentStep : 0));
}

QVariant::Type BSettingsNode::type() const
{
    return d_func()->type;
}

BSettingsNode::SetFunction BSettingsNode::userSetFunction() const
{
    if (d_func()->setFunction)
        return d_func()->setFunction;
    BSettingsNode *pn = d_func()->parentNode;
    return pn ? pn->userSetFunction() : 0;
}

BSettingsNode::ShowFunction BSettingsNode::userShowFunction() const
{
    if (d_func()->showFunction)
        return d_func()->showFunction;
    BSettingsNode *pn = d_func()->parentNode;
    return pn ? pn->userShowFunction() : 0;
}
