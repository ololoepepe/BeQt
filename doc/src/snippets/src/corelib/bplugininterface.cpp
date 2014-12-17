/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the documentation of the BeQt library.
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

//! [0]
#include <BPluginInterface>
#include <BTerminal>
#include <BVersion>

#include <QString>
#include <QStringList>
#include <QtPlugin>

class MyPlugin : public QObject, public BPluginInterface
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "TeX-Creator.PretexEditorModulePlugin")
#endif
    Q_OBJECT
    Q_INTERFACES(BPluginInterface)
public:
    explicit MyPlugin() {}
public:
    void activate()
    {
        BTerminal::installHandler("sum", &myHandler);
    }
    void deactivate() {}
    QString id() const { return "myplugin"; }
    PluginInfo info() const { return PluginInfo(); }
    bool prefereStaticInfo() const { return true; }
    StaticPluginInfo staticInfo() const
    {
        StaticPluginInfo info;
        info.authors << PersonInfo("Andrey Bogdanov", "Main developer");
        info.copyrightYears = "2014";
        info.organization = "myorg";
        info.website = "myorg.com";
        return info;
    }
    QString title() const { return tr("My Plugin"); }
    QString type() const { return "terminal-handler"; }
    BVersion version() const { return BVersion(1, 0, 0); }
private:
    static bool myHandler(const QString &, const QStringList &args)
    {
        if (args.isEmpty()) {
            bWriteLine(tr("Not enough arguments"));
            return false;
        }
        double d = 0.0;
        foreach (const QString &s, args) {
            bool ok = false;
            double dd = s.toDobule(&ok);
            if (!ok) {
                bWriteLine(tr("Invalid argument"));
                return false;
            }
            d += dd;
        }
        bWriteLine(tr("The sum is:") + " " + QString::number(d));
        return true;
    }
};
//! [0]
