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
    QVariant defaultValue;
    BTranslation description;
    BSettingsNode::GetFunction getFunction;
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
    defaultValue = QVariant(type);
    secure = false;
    getFunction = 0;
    setFunction = 0;
    showFunction = 0;
}

/*============================================================================
================================ BSettingsNode ===============================
============================================================================*/

/*!
\class BSettingsNode
\inmodule BeQtCore
\brief The BSettingsNode class is used to provide information about application settings hierarchy for BTerminal.

Application settings hierarchy is represented by several BSettingsNode objects organized in a tree structure. There is
one root node, and it's child nodes, which in turn may have their own child nodes.

Each BSettingsNode represents either a section, a subsection, or a key. The exception is the root node. Every key node
has it's type (one of the QVariant::Type). A node may also provide custom setter/getter functions, and a function for
showing the corresponding value in a terminal.

\sa {Settings handling}
*/

/*!
\typedef BSettingsNode::GetFunction

The BSettingsNode::GetFunction typedef provides a pointer to a function taking const pointer to BSettingsNode and
returning QVariant.
*/

/*!
\typedef BSettingsNode::SetFunction

The BSettingsNode::SetFunction typedef provides a pointer to a function taking const pointer to BSettingsNode and const
QVariant, and returning bool.
*/

/*!
\typedef BSettingsNode::ShowFunction

The BSettingsNode::ShowFunction typedef provides a pointer to a function taking const pointer to BSettingsNode and const
QVariant, and returning bool.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a settings node and sets it's parent to \a parent.
*/

BSettingsNode::BSettingsNode(BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
}

/*!
Constructs a settings node and sets it's parent to \a parent. The node's key is set to \a key.
*/

BSettingsNode::BSettingsNode(const QString &key, BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
    setKey(key);
}

/*!
Constructs a settings node and sets it's parent to \a parent. The node's key is set to \a key, and the value type is
set to \a type.
*/

BSettingsNode::BSettingsNode(QVariant::Type type, const QString &key, BSettingsNode *parent) :
    BBase(*new BSettingsNodePrivate(this))
{
    d_func()->init();
    setParent(parent);
    setType(type);
    setKey(key);
}

/*!
Destroys the object, deleting the associated data object. All child objects are deleted recursively.
*/

BSettingsNode::~BSettingsNode()
{
    //
}

/*============================== Static public methods =====================*/

/*!
Converts the string \a s to a QVariant of type \a t. Returns that QVariant.

If \a ok if a non-null pointer, it's value is set to true if the conversion is successful, otherwise it's value is set
to false.

\sa variantToString()
*/

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

/*!
Returns the string representation of the QVariant \a v.

If \a ok is a non-null pointer and \a v can be represented as a string, the value of \a ok is set to true, otherwise
it's value is set to false.

\sa stringToVariant()
*/

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

/*!
Appends \a node to the list of this node's child nodes.

\sa childNodes(), removeChild()
*/

void BSettingsNode::addChild(BSettingsNode *node)
{
    if (!node || d_func()->childNodes.contains(node))
        return;
    node->setParent(this);
}

/*!
\overload
Creates a new BSettingsNode with the type \a type and key \a key, and appends it to the list of this node's child
nodes.

\sa childNodes(), removeChild()
*/

void BSettingsNode::addChild(const QString &key, QVariant::Type type)
{
    addChild(new BSettingsNode(type, key));
}

/*!
Returns the number of child nodes of this node.

\sa childNodes()
*/

int BSettingsNode::childCount() const
{
    return d_func()->childNodes.size();
}

/*!
Returns the list of child nodes of this node.

\sa childCount()
*/

QList<BSettingsNode *> BSettingsNode::childNodes() const
{
    return d_func()->childNodes;
}

/*!
Creates a copy of this node. Returns a pointer to the new node.

All child nodes are copied recursively.

The parent of the new node is set to \a parent.
*/

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

/*!
Returns the default value for this node.

\sa setDefaultValue()
*/

QVariant BSettingsNode::defaultValue() const
{
    return d_func()->defaultValue;
}

/*!
Returns the description of this node.

The description is used by BTerminal when showing help.

\sa setDescription(), BTerminal
*/

BTranslation BSettingsNode::description() const
{
    return d_func()->description;
}

/*!
Searches for the settings node with path \a path recursively.

\a separator is used as a (sub)section separator.

Returns a pointer to the node if it is found, otherwise returns 0.
*/

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

/*!
\overload
Does the same as find(), but returns a const pointer.
*/

const BSettingsNode *BSettingsNode::find(const QString &path, QChar separator) const
{
    return const_cast<BSettingsNode *>(this)->find(path, separator);
}

/*!
Returns the key of this node.

\sa setKey()
*/

QString BSettingsNode::key() const
{
    return d_func()->key;
}

/*!
Returns the parent of this node.

\sa setParent()
*/

BSettingsNode *BSettingsNode::parent() const
{
    return d_func()->parentNode;
}

/*!
Removes \a node form the list of this node's child nodes.

\sa addChild()
*/

void BSettingsNode::removeChild(BSettingsNode *node)
{
    if (!node)
        return;
    node->setParent(0);
}

/*!
Returns true if the setting represented by this node must be set securely (i.e. with the terminal echo being enabled).

\sa setSecureInput()
*/

bool BSettingsNode::secureInput() const
{
    return d_func()->secure;
}

/*!
Requests user input of the value for settings node with path \a path.

\a separator is used as a (sub)section separator.

Returns true if a node with the path specified exists and if it's setting value is retrieved successfully; otherwise
returns false.
*/

bool BSettingsNode::set(QString path, QChar separator) const
{
    return set(path, tr("Enter value for") + " \"%k\": ", separator);
}

/*!
\overload
Requests user input of the value for settings node with path \a path.

\a separator is used as a (sub)section separator.

Returns true if a node with the path specified exists and if it's setting value is retrieved successfully; otherwise
returns false.

\a text is used as a prompt template. "%k" is replaced with the node key.

Example template:

"Value for %k: "
*/

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

/*!
\overload
Sets the value for settings node with path \a path to \a value.

\a separator is used as a (sub)section separator.

Returns true if a node with the path specified exists and if it's setting value is set successfully; otherwise returns
false.
*/

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

/*!
Sets the default value for the setting represented by this node to \a value.

\sa defaultValue()
*/

void BSettingsNode::setDefaultValue(const QVariant &value)
{
    if (value.type() != d_func()->type) {
        d_func()->defaultValue = QVariant(d_func()->type);
        return;
    }
    d_func()->defaultValue = value;
}

/*!
Sets the description of this node to \a t.

\sa description()
*/

void BSettingsNode::setDescription(const BTranslation &t)
{
    d_func()->description = t;
}

/*!
Sets the key of this node to \a key.

\sa key()
*/

void BSettingsNode::setKey(const QString &key)
{
    d_func()->key = key;
}

/*!
Sets the parent of this node to \a parent.

\sa parent()
*/

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

/*!
Sets wether the setting represented by this node must be set securely (i.e. with the terminal echo being enabled).

If \a b is true, the setting must be set securely; otherwise it must be set with terminal echo enabled.

\sa secureInput()
*/

void BSettingsNode::setSecureInput(bool b)
{
    d_func()->secure = b;
}

/*!
Sets the type of the setting represented by this node to \a type.

\sa type()
*/

void BSettingsNode::setType(QVariant::Type type)
{
    d_func()->type = type;
    d_func()->defaultValue = QVariant(type);
}

/*!
Sets a pointer to the custom function used to get the value of the setting represented by this node to \a f.

\sa userGetFunction(), setUserSetFunction(), setUserShowFunction()
*/

void BSettingsNode::setUserGetFunction(GetFunction f)
{
    d_func()->getFunction = f;
}

/*!
Sets a pointer to the custom function used to set the value of the setting represented by this node to \a f.

\sa userSetFunction(), setUserGetFunction(), setUserShowFunction()
*/

void BSettingsNode::setUserSetFunction(SetFunction f)
{
    d_func()->setFunction = f;
}

/*!
Sets a pointer to the custom function used to show the value of the setting represented by this node to \a f.

\sa userShowFunction(), setUserGetFunction(), setUserSetFunction()
*/

void BSettingsNode::setUserShowFunction(ShowFunction f)
{
    d_func()->showFunction = f;
}

/*!
Prints to standard output (stdin) the value of settings node with path \a path.

\a separator is used as a (sub)section separator.

Returns true if a node with the path specified exists and if it's setting value is printed successfully; otherwise
returns false.
*/

bool BSettingsNode::show(QString path, QChar separator) const
{
    return show(path, tr("The value for") + " \"%k\": %v", separator);
}

/*!
\overload
Prints to standard output (stdin) the value of settings node with path \a path.

\a separator is used as a (sub)section separator.

Returns true if a node with the path specified exists and if it's setting value is printed successfully; otherwise
returns false.

\a text is used as a template. "%k" is replaced with the node key, and "%v" is replaced with the setting value.

Example template:

"%k = %v"
*/

bool BSettingsNode::show(QString path, QString text, QChar separator) const
{
    const BSettingsNode *n = find(path, separator);
    if (!n)
        return false;
    if (separator.isNull())
        separator = '.';
    path.replace(separator, '/');
    QVariant v = n->userGetFunction() ? n->userGetFunction()(n) : bSettings->value(path, n->defaultValue());
    if (n->userShowFunction())
        return n->userShowFunction()(n, v);
    bool ok = false;
    QString vs = variantToString(v, &ok);
    if (!ok)
        return false;
    BTerminal::writeLine(text.replace("%k", path.split("/").last()).replace("%v", vs));
    return true;
}

/*!
Prints to standard output (stdin) the settings hierarchy in a tree-like form.

\a indentStep is the number of spaces prepended to every new hierarchy level.

\a initialIndent is the number of spaces prepended to every printed line.
*/

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

/*!
Returns the typ of the value of the setting represented by this node.

\sa setType()
*/

QVariant::Type BSettingsNode::type() const
{
    return d_func()->type;
}

/*!
Returns a pointer to the custom function used to get the value of the setting represented by this node.

\sa setUserGetFunction(), userSetFunction(), userShowFunction()
*/

BSettingsNode::GetFunction BSettingsNode::userGetFunction() const
{
    return d_func()->getFunction;
}

/*!
Returns a pointer to the custom function used to set the value of the setting represented by this node.

\sa setUserSetFunction(), userGetFunction(), userShowFunction()
*/

BSettingsNode::SetFunction BSettingsNode::userSetFunction() const
{
    if (d_func()->setFunction)
        return d_func()->setFunction;
    BSettingsNode *pn = d_func()->parentNode;
    return pn ? pn->userSetFunction() : 0;
}

/*!
Returns a pointer to the custom function used to show the value of the setting represented by this node.

\sa setUserShowFunction(), userGetFunction(), userSetFunction()
*/

BSettingsNode::ShowFunction BSettingsNode::userShowFunction() const
{
    if (d_func()->showFunction)
        return d_func()->showFunction;
    BSettingsNode *pn = d_func()->parentNode;
    return pn ? pn->userShowFunction() : 0;
}
