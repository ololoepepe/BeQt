==============================================================================
 BeQt - Beyond Qt
==============================================================================

Check for your language README in the "readme" directory.

Homepage: https://github.com/the-dark-angel/BeQt

Author: Andrey Bogdanov (ololoepepe@gmail.com)

License: GNU LGPL (see COPYING.txt)

BeQt is a library set developed as an extension of Qt.
That is where the project's name comes from. BeQt may be useful
in small and medium desktop applications with classic interface.
It takes most of routine work, allowing one to focus
on solving the actual task.

BeQt needs you! Everyone may contribute to this open project.
If you wish to get involved, or have any ideas or suggestions,
or you need help dealing with BeQt, - you may contact me
by e-mail or Jabber (ololoepepe@gmail.com).

==============================================================================
 Overview
==============================================================================

BeQt modules help one to quickly implement working with application resources,
plugin support, and user interface on-the-fly translation.

BeQt provides the means for convenient user commands handling in
console applications, and even it's own terminal emulator.
Several frequently used auxiliary widgets are also provided.

Moreover, BeQt implements a ready-to-use network client-server
interaction model, based on Qt classes.

Entire module is dedicated to a simple yet powerfull text editor,
capable of selecting text by rectangular blocks.

Important: BeQt does not deal with QtQuick.

==============================================================================
 Dependencies
==============================================================================

In order to build and use BeQt you will need the same libraries and tools
as for every other project using Qt.

See: http://qt-project.org/resources/getting_started for details.

BeQt is intended for use with Qt 5, or with Qt 4.8. Classes interface may
slightly vary depending on the version of Qt used. In case of using Qt 4.8,
some features may be missing.
Support of Qt libraries' versions lower than 4.8.0 is not guaranteed.

Note: in order to use all BeQt features you will need the following Qt
modules: QtCore, QtConcurrent, QtNetwork, QtGui, QtSql, QtWidgets (for Qt 5).

In order to use builtin .svgz icons you will also need
the corresponding plugin.

See: http://qt-project.org/doc/qt-5.0/plugins-howto.html for details.

==============================================================================
 Building and installing (briefly)
==============================================================================

In order to build BeQt, just cd into the sources directory and execute
the following commands:

 * "qmake"

 * "make"
   or other similar command ("nmake", "mingw32-make", etc.)

 * "make install"
   You may need the superuser privileges in UNIX-like systems 

Note: When building from the command line, you have to configure
the environment (path to Qt libraries, etc.).

You may also use Qt Creator. After building the project, cd to
the output directory and execute the "make install" command, or
configure automatic execution of that command in Qt Creator.

See: http://qt-project.org/doc/qtcreator-2.6 for details.

==============================================================================
 Building and installing (in detail)
==============================================================================

When building BeQt, you may pass some parameters to qmake:

 * "HUNSPELL_PREFIX=<path>"
   Set external Hunspell library location. By default the internal one is used

 * "CONFIG+=no_network"
   Don't build the BeQtNetwork module

 * "CONFIG+=no_sql"
   Don't build the BeQtSql module

 * "CONFIG+=no_widgets"
   Don't build the BeQtWidgets module (BeQtCodeEditor module also will not be
   built, because it depends on BeQtWidgets module)

 * "CONFIG+=no_codeeditor"
   Don't build the BeQtCodeEditor module

 * "CONFIG+=no_networkwidgets"
   Don't build the BeQtNetworkWidgets module

 * "CONFIG+=builtin_resources"
   Embed resources (including translations) into library files
   See: http://qt-project.org/doc/qt-5.0/resources.html for details

 * "CONFIG+=examples"
   Build demo examples (by default examples are not built)

 * "CONFIG+=no_install"
   Don't install any files (building only)

 * "CONFIG+=no_headers"
   Don't install headers

 * "CONFIG+=private_headers"
   Install private headers along with the public ones

 * "BEQT_PREFIX=<path>"
   Set install path. See the note below

 * "BEQT_HEADERS_INSTALLS_PATH=<path>"
   Set install path for headers. See the note below

 * "BEQT_LIBS_INSTALLS_PATH=<path>"
   Set install path for library files. See the note below

 * "BEQT_RESOURCES_INSTALLS_PATH=<path>"
   Set install path for resources. See the note below

 * "BEQT_SUBDIR_NAME=<name>"
   Set name of the BeQt subdirectory when BeQt is used as a submodule

Note: In Windows systems BeQt is installed to the "C:\Program files\BeQt"
directory (or other similar directory, depending on your system) by default.
Headers, libraries and resources are installed
to the corresponding subdirectories (include, lib, etc.).

In UNIX-like systems the directory structure during installation
looks like this:

 * "/usr/include/beqt"
   Path to headers

 * "/usr/lib"
   Path to libraries

 * "/usr/share/beqt"
   Path to resources

You may set another installation path. To do so,
pass the corresponding parameter to qmake (see above).

Warning: Don't use paths containing spaces. In Windows systems you may replace
directory names, containing spaces, with the corresponding short names:

See: http://support.microsoft.com/kb/142982 for details.

==============================================================================
 Using
==============================================================================

BeQt may be used either as a system library, or as a project submodule.

Important: BeQt may be linked to a project with just two lines of code in the
.pro file, but in that case you need to use qmake.

First of all, add the following to your .pro file:

 "BEQT = [core] [network] [sql] [widgets] [codeeditor] [networkwidgets] [all]"

There are BeQt module names in the square brackets.
You may also add all modules (the "[all]" parameter).
BeQt and Qt modules' dependencies are resolved automatically.

In case BeQt is used as a system library (or is built and
placed to a separate directory), you need to add the following line to your
.pro file after "BEQT = ...":

 "include(<path_to_beqt>/depend.pri)"

"<path_to_beqt>" is a path to BeQt resources (in UNIX-like systems it is
usually "/usr/share/beqt", and in Windows it is "C:\Program files\BeQt").

In case BeQt is used as a submodule, you will have to make your
project structure slightly more compilcated, creating a common .pro file,
in which both BeQt and your project will be included as subprojects.

For details, see:
http://qt-project.org/doc/qt-5.0/qmake-project-files.html#project-templates

If you compiled BeQt with external Hunspell library, you must specify the
path to Hunspell again when linking BeQt:

 "HUNSPELL_PREFIX=<path>"

Common .pro file example:

 "...
 CONFIG+=ordered
 TEMPLATE=subdirs
 SUBDIRS=beqt yourproject
 ..."

Important: Note, that the BeQt submodule must be included before any other
submodule that uses it, and the "ordered" parameter must be passed to
the "CONFIG" variable. It's done to guarantee that BeQt is built
before any other submodule uses it.

Note: Due to qmake features you may have to execute "qmake" and "make"
commands again after building the project (it's required for proper library
linking). The repeated command execution will take at most few seconds.

When building, BeQt generates translation files (.qm), containing translations
of UI elements. These files are located in the "translations" subdirectory
(for example, "translations/beqt_ru.qm").

If you wish to use BeQt whith different versions of Qt, the following link
may be useful for you:
http://qt-project.org/doc/qt-5.0/qtcore/qtglobal.html#QT_VERSION_CHECK

==============================================================================
 FAQ
==============================================================================

Q: Ho do I start using BeQt?
A: See "Building and installing", "Using",
and demos in the "examples" directory.

В: Why the BeQtWidgets module is called so? Qt 4.8 has no Widgets module!
О: In Qt 5, all widgets were moved to a separate module (QtWidgets).

Q: What are BeQt license restrictions?
A: There are almost no restrictions. You may use BeQt as you wish,
but don't forget that this statement doesn't apply to the Qt libraries.
See: COPYING.txt, http://qt-project.org/products/licensing for details.

Q: I'm having troubles using BeQt, where can I get help?
A: E-mail/Jabber: ololoepepe@gmail.com

Q: I've detected a bug/I have an idea, where can I report/post it?
A: See the answer above.

Q: Why there is no documentation?
A: Unfoturnately, I just don't have enough time to write documentation.
You may help the project if you get involved (see the answer above).

Q: Will the project be maintained/developed further?
A: Current version (2.x.y) is likely to be maintained. Further project
development depends on whether there are anybody willing to help me with it.
Unfoturnately, I don't have enough time and skills to develop
such a big project further on my own.

Q: Does it make sense to use BeQt together with QtQuick (QML)?
A: BeQtCore and BeQtNetwork modules may be useful
when creating UI with QtQuick.

==============================================================================
 Deploying
==============================================================================

When deploying an application that uses BeQt, don't forget to include
the resource files (libraries, images, translations, etc. - see above).

It's recommended to build applications statically, because that helps avoid
dependency issues. You may use the "builtin_resources" parameter
in case of building your project statically.

For details, see:
http://qt-project.org/doc/qt-5.0/deployment.html#static-vs-shared-libraries
