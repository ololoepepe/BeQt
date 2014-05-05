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

#ifndef BSETTINGSNODE_H
#define BSETTINGSNODE_H

class BSettingsNodePrivate;

#include "bglobal.h"
#include "bbase.h"
#include "btranslation.h"

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
    typedef bool (*SetFunction)(const BSettingsNode *node, const QVariant &value);
    typedef bool (*ShowFunction)(const BSettingsNode *node, const QVariant &value);
public:
    explicit BSettingsNode(BSettingsNode *parent = 0);
    explicit BSettingsNode(const QString &key, BSettingsNode *parent = 0);
    explicit BSettingsNode(QVariant::Type type, const QString &key, BSettingsNode *parent = 0);
    ~BSettingsNode();
public:
    static QVariant stringToVariant(const QString &s, QVariant::Type t, bool *ok = 0);
    static QString variantToString(QVariant v, bool *ok = 0);
public:
    void setParent(BSettingsNode *parent);
    void setKey(const QString &key);
    void setType(QVariant::Type type);
    void setSecureInput(bool b);
    void setDescription(const BTranslation &t);
    void setUserSetFunction(SetFunction f);
    void setUserShowFunction(ShowFunction f);
    void addChild(BSettingsNode *node);
    void addChild(const QString &key, QVariant::Type type = QVariant::String);
    void removeChild(BSettingsNode *node);
    void showTree(int indentStep = 2, int initialIndent = 0) const;
    BSettingsNode *find(const QString &path, QChar separator = QChar('.'));
    const BSettingsNode *find(const QString &path, QChar separator = QChar('.')) const;
    bool set(QString path, QChar separator = QChar('.')) const;
    bool set(QString path, QString text, QChar separator = QChar('.')) const; //%k - key
    bool set(QString path, QVariant value, QChar separator = QChar('.')) const;
    bool show(QString path, QChar separator = QChar('.')) const;
    bool show(QString path, QString text, QChar separator = QChar('.')) const; //%k - key, %v - value
    BSettingsNode *clone(BSettingsNode *parent = 0) const;
    BSettingsNode *parent() const;
    QList<BSettingsNode *> childNodes() const;
    int childCount() const;
    QString key() const;
    QVariant::Type type() const;
    bool secureInput() const;
    BTranslation description() const;
    SetFunction userSetFunction() const;
    ShowFunction userShowFunction() const;
private:
    Q_DISABLE_COPY(BSettingsNode)
};

#endif // BSETTINGSNODE_H
