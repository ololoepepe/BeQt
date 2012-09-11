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

beqtcore provides a namespace BCore, containing some useful constants (such as
const int Kilobyte = 1024), some methods for handling plugins, and other useful
methods.

beqtgui implements a main window widget with some features common for most of
applications. It also provides a minimalistic yet powerfull text editor, and
some other widgets.

beqtnetwork helps you deal with TCP sockets. It provides a TCP socket-based
classes, which are much more abstracted than the ones provided by Qt.

===============================================================================
 Requirements
===============================================================================

To build and use BeQt you will need the same libraries and tools as for
building any other Qt-based project.
See: http://qt-project.org/resources/getting_started for details.

It is strongly recommended that you use Qt libraries 4.8.0 or higher. Further
releases of BeQt may use Qt libraries 5.0.
The easiest way to get Qt is to download the SDK.

===============================================================================
 Building and installing
===============================================================================

BeQt may only be built as shared libraries (".dylib" on Mac, ".so" on other
UNIX-like systems and ".dll" on Windows). This is because there are some issues
when linking one static library to another.

The first possibility is to use Qt Creator with the appropriate libraries, and
then use the install script only. In this case, you should specify the "build"
subdirectory as the target directory. After building the project, simply run
"./install.sh" (on UNIX-like systems) or "install.bat" (on Windows).

The other possibility is to use the terminal. You have to configure your "PATH"
variable correctly, or configure the terminal session to use the building
script. On Windows, for example, you may use the special configuration batch
files from the Qt SDK. After configuring the system environment (or terminal
session) do the following:

  1. On UNIX-like systems:

    In terminal, cd to the BeQt source files directory and run:
    "./build-install.sh"

  2. On Windows:

    In terminal, cd to the BeQt source files directory and run:
    "./build-install.sh [your_make_command] [make_parameters]", where
    your_make_command is the appropriate make command, or mingw32-make if no
    command is specified, and make_parameters is the list of make parameters
    (up to 4).

On UNIX-like systems the library files will be located in the "/usr/lib"
directory, and the headers will be located in the "/usr/include/beqt"
directory.

On Windows all files will be located in the "%systemdrive%\Program files\BeQt"
directory ("%systemdrive%" usually stands for "C:", but some systems may be
located on other drives, for example "D:"). The following subdirectories will
be created:

 * "lib" - library files will be located here, for example "beqtcore0.dll"
 * "include" - headers will be located here, for example "bcore.h"

===============================================================================
 Using
===============================================================================

In order to use BeQt, you first have to specify the paths to libraries and
headers in your project (".pro") files.

On UNIX-like systems the paths will be:

 * "unix:LIBS += -L"/usr/lib/beqt" -l<name1> -l<name2>",
   where "<name1>" and "<name2>" are library names.
 * "unix:INCLUDEPATH += "/usr/include/beqt"

On Windows the paths will be:

 * "win32:LIBS += -L"$$(systemdrive)/Program files/BeQt/lib" -l<name>0"
   Note: The "0" is a part of the library file name, for example "beqtcore0.dll".
 * "win32:INCLUDEPATH += "$$(systemdrive)/Program files/BeQt/include""

Now, to use the classes, provided by BeQt, you may type:

 "#include <classname.h>"
 
in your source files.

Remember that all libraries depends on beqtcore. Also, remeber that you should
add the correspoding Qt module to your project for each library.

To include a module, add the following line to your project's ".pro" file:
"QT += <name>", where "<name>" stands for module name.
