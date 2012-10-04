===============================================================================
 BeQt - Beyond Qt
===============================================================================

Homepage: https://github.com/the-dark-angel/BeQt

Author: Andrey Bogdanov

License: See COPYING.txt

BeQt is a library set, developed as an extension of Qt (or you may say
"a framework in a framework"). That is where the project's name comes from.

BeQt is suitable for small and medium desktop applications, implementing such
common features as:

 * Translating UI
 * Loading plugins
 * Showing "About", "Help" and "Settings" dialogs
 * Storing basic application settings

===============================================================================
 Overview
===============================================================================

BeQt consists of three libraries: beqtcore, beqtgui and beqtnetwork. These
libraries correspond to QtCore, QtGui and QtNetwork modules, respectively.

beqtcore provides a class BCore, containing some useful constants (such as
const int Kilobyte = 1024), some methods for handling plugins, and other useful
methods.

beqtgui implements a main window widget with some features common for most of
applications. It also provides a minimalistic yet powerfull text editor, and
some other widgets.

beqtnetwork helps you deal with network connections via the client-server
model. It provides socket-based classes, which are much more abstracted than
the ones provided by Qt.

===============================================================================
 Requirements
===============================================================================

To build and use BeQt you will need the same libraries and tools as for
building any other Qt-based project.
See: http://qt-project.org/resources/getting_started for details.

You have to use Qt libraries version 4.8.x.
Further releases of BeQt may use Qt libraries 5.0.
The easiest way to get Qt is to download the SDK.

===============================================================================
 Building and installing
===============================================================================

BeQt may only be built as shared libraries (".dylib" on Mac, ".so" on other
UNIX-like systems and ".dll" on Windows). This is because there are some issues
when linking one static library to another.

All you have to do is run the followong commands:

 * qmake
 * make
   or other make command
 * make install
   You may have to run this command as a superuser on UNIX-like systems.

You may also use Qt Creator. Just open the beqt.pro file, and build the
libraries (configuration must be set to "Release"). After that, use the
"make install" command.

On UNIX-like systems the library files will be located in the "PREFIX/lib"
directory, the headers will be located in the "PREFIX/include/beqt" directory,
and the resources will be located in the "PREFIX/share/beqt" directory.

On Windows all files will be located in the "PREFIX" directory.
The following subdirectories will be created:

 * "lib" - library files will be located here, for example "beqtcore1.dll"
 * "include" - headers will be located here, for example "bcore.h"
 * "translations" - translation files (.qm)

On UNIX-like systems, PREFIX defaults to "/usr". On Windows it will be
"C:\PROGRA~1\BeQt". You may change PREFIX value when running qmake. To do this,
pass "PREFIX=install_path" parameter to qmake.
Warning: don't use spaces. Use short names instead. For example, use "PROGRA~1"
instead of "Program files".

===============================================================================
 Using
===============================================================================

In order to use BeQt, you first have to specify the paths to libraries and
headers in your project (".pro") files.

Remember that all libraries depends on beqtcore. Also, remeber that you should
add the correspoding Qt module to your project for each library.

If you wish to use the translation or plugins system, or the BMainWindow class,
you have to create an instance of QCoreApplication (or QApplication for GUI
programms), and then call BCore::init(). After that, you may call
BCore::loadSettings(), and then you may create your objects and call
QApplication::exec(). If you wish to save BCore settings, you must call
BCore::saveSettngs() before returning from the main() function.

===============================================================================
 Deploying
===============================================================================

When deploying a BeQt-based application, you should remember to supply BeQt
library files and resources. You should create some directories for that.

Let's assume you are deploying the "myapp" application.

On UNIX-like systems the directories will be:

 * "/usr/share/myapp/translations"

On Windows the directories will be:

 * "C:\Program files\myapp\translations"

You should put the BeQt translation files to the "translations" subdirectory of
your application. See Building and installing section for BeQt resources
location.
