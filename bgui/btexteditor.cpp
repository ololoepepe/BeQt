#include "btexteditor.h"
#include "bsyntax.h"
#include "babstractsettingstab.h"
#include "babstractfiletype.h"

#include "private/bguicommon.h"
#include "private/btexteditordocument.h"
#include "private/bencoding.h"
#include "private/bselectfilesdialog.h"
#include "private/bfinddialog.h"
#include "private/bopensavedialog.h"
#include "private/btexteditorsettingstab.h"
#include "private/bmacrorecorder.h"

#include "../bcore/bcore.h"

#include <QWidget>
#include <QSignalMapper>
#include <QAction>
#include <QMap>
#include <QList>
#include <QEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTabWidget>
#include <QToolBar>
#include <QString>
#include <QIcon>
#include <QKeySequence>
#include <QPlainTextEdit>
#include <QFileInfo>
#include <QVariant>
#include <QFileDialog>
#include <QMenu>
#include <QToolButton>
#include <QCursor>
#include <QMessageBox>
#include <QClipboard>
#include <QFile>
#include <QTextStream>
#include <QChar>
#include <QStringList>
#include <QScopedPointer>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QToolButton>
#include <QLabel>
#include <QFont>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QColor>
#include <QBrush>
#include <QSettings>
#include <QDir>
#include <QSize>
#include <QVariantMap>
#include <QDesktopServices>

#include <QDebug>

class AllFiles : public BAbstractFileType
{
public:
    AllFiles() {}
    //
    QString description() const {return tr("All files", "fileType description");}
    QStringList suffixes() const {return QStringList();}
};

class UTF8 : public BEncoding
{
public:
    QString codecName() const { return "UTF-8"; }
    QString descriptiveName() const { return QObject::tr("Unicode", "codec descriptiveName"); }
};

class UTF16 : public BEncoding
{
public:
    QString codecName() const { return "UTF-16"; }
    QString descriptiveName() const { return QObject::tr("Unicode", "codec descriptiveName"); }
};

class UTF32 : public BEncoding
{
public:
    QString codecName() const { return "UTF-32"; }
    QString descriptiveName() const { return QObject::tr("Unicode", "codec descriptiveName"); }
};

class KOI8R : public BEncoding
{
public:
    QString codecName() const { return "KOI8-R"; }
    QString descriptiveName() const { return QObject::tr("Cyrillic", "codec descriptiveName"); }
};

class KOI8U : public BEncoding
{
public:
    QString codecName() const { return "KOI8-U"; }
    QString descriptiveName() const { return QObject::tr("Cyrillic (Ukrainian)", "codec descriptiveName"); }
};

class Windows1251 : public BEncoding
{
public:
    QString codecName() const { return "Windows-1251"; }
    QString descriptiveName() const { return QObject::tr("Cyrillic", "codec descriptiveName"); }
};

//

#if defined(Q_OS_LINUX)
const QString DefFontFamily = "Monospace";
#elif defined(Q_OS_WIN)
const QString DefFontFamily = "Monaco";
#elif defined(Q_OS_MAC)
const QString DefFontFamily = "Courier";
#endif
//
const QString DefMacrosDir = BCore::Home;
const QString DefEncoding = "UTF-8";
const int DefFontPointSize = 10;
const int DefLineLength = 120;
const int DefTabWidth = 4;
const QString DefFileDialogDir = BCore::Home;
const int MessageTimeout = 15 * BCore::Second;
const QStringList AvailableEncodings = QStringList() << "UTF-8"; //TODO
const int _m_MaxRecentFiles = 15;
//_m_GroupTextEditor
  const QString _m_KeyDir = "dir";
  const QString _m_KeyMacrosDir = "macros_dir";
  const QString _m_KeyFontFamily = "font_family";
  const QString _m_KeyFontSize = "font_size";
  const QString _m_KeyLineLength = "line_length";
  const QString _m_KeyTabWidth = "tab_width";
  const QString _m_KeyMessageTimeout = "message_timeout";
  const QString _m_KeyDefaultCodecName = "default_codec_name";
  const QString _m_KeyRecentFiles = "recent_files";

//

QList<BTextEditor *> _m_editors; //TODO

//

bool checkReadOnly(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    QFile f(fileName);
    bool ro = !f.open(QFile::WriteOnly | QFile::Append);
    f.close();
    return ro;
}

//

bool BTextEditor::isFileOpenedGlobal(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    for (int i = 0; i < _m_editors.size(); ++i)
        if ( _m_editors.at(i)->isFileOpened(fileName) )
            return true;
    return false;
}

QStringList BTextEditor::openedFilesGlobal(const QStringList &fileNames)
{
    QStringList sl;
    for (int i = 0; i < _m_editors.size(); ++i)
        sl << _m_editors.at(i)->openedFiles(fileNames);
    return sl;
}

void BTextEditor::applySettingsGlobal(const QVariantMap &settings)
{
    for (int i = 0; i < _m_editors.size(); ++i)
        _m_editors.at(i)->applySettings(settings);
}

//

BTextEditor::BTextEditor(QWidget *parent, const QString &id) :
    QWidget(parent), _m_CId(id)
{
    _m_init();
    _m_retranslateUi();
    _m_loadSettings();
    _m_editors << this;
}

BTextEditor::~BTextEditor()
{
    delete _m_defaultFileType;
    _m_editors.removeAll(this);
    if (_m_wgtIndicator)
        _m_wgtIndicator->deleteLater();
}

//

bool BTextEditor::eventFilter(QObject *object, QEvent *event)
{
    switch ( event->type() )
    {
    case QEvent::DragEnter:
        static_cast<QDragEnterEvent *>(event)->acceptProposedAction();
        return true;
    case QEvent::DragMove:
        static_cast<QDragMoveEvent *>(event)->acceptProposedAction();
        return true;
    case QEvent::DragLeave:
        static_cast<QDragLeaveEvent *>(event)->accept();
        return true;
    case QEvent::Drop:
        _m_handleUrls( static_cast<QDropEvent *>(event)->mimeData()->urls() );
        return true;
    default:
        break;
    }
    return QWidget::eventFilter(object, event);
}

void BTextEditor::setAcceptableFileTypes(const FileTypeInfoList &list)
{
    _m_fileTypes = list;
}

void BTextEditor::setMacrosDefaultDir(const QString &dir)
{
    if ( dir.isEmpty() )
        return;
    _m_macrosDefDir = dir;
}

void BTextEditor::setDefaultEncoding(const QString &codecName)
{
    if ( codecName.isEmpty() )
        return;
    _m_codecName = codecName;
    if ( _m_documents.isEmpty() )
        _m_updateEncoding(_m_codecName);
}

void BTextEditor::setFontFamily(const QString &family)
{
    //no check for font being monospace is provided by Qt. sad but true
    if (family == _m_fontFamily)
        return;
    _m_fontFamily = family;
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setFontFamily(_m_fontFamily);
}

void BTextEditor::setFontPointSize(int pointSize)
{
    if (pointSize < 1 || pointSize == _m_fontPointSize)
        return;
    _m_fontPointSize = pointSize;
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setFontPointSize(_m_fontPointSize);
}

void BTextEditor::setLineLength(int length)
{
    if (length < 10 || length == _m_lineLength)
        return;
    _m_lineLength = length;
    _m_findDlg->setLineLength(_m_lineLength);
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setLineLength(_m_lineLength);
}

void BTextEditor::setTabWidth(int width)
{
    if (width < 2 || width == _m_tabWidth)
        return;
    _m_tabWidth = width;
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setTabWidth(_m_tabWidth);
}

void BTextEditor::setMessageTimeout(int timeout)
{
    if (timeout > 0)
        _m_messageTimeout = timeout;
}

void BTextEditor::setSyntaxes(const QList<BSyntax> &list)
{
    _m_syntaxes.clear();
    _m_cmboxSyntax->clear();
    _m_syntaxes = list;
    bool b = false;
    BSyntax s;
    for (int i = 0; i < list.size(); ++i)
    {
        if ( list.at(i).type() == s.type() )
        {
            b = true;
            break;
        }
    }
    if (!b)
        _m_syntaxes.prepend(s);
    for (int i = 0; i < _m_syntaxes.size(); ++i)
        _m_cmboxSyntax->addItem( _m_syntaxes.at(i).type() );
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_setDocumentSyntax( _m_documents.at(i) );
}

void BTextEditor::loadSyntaxes(const QString &dir)
{
    QDir d(dir);
    if ( !d.exists() )
        return;
    _m_syntaxes.clear();
    _m_cmboxSyntax->clear();
    _m_syntaxes << BSyntax();
    _m_cmboxSyntax->addItem( _m_syntaxes.first().type() );
    QStringList sl = d.entryList(QStringList() << "*.xml", QDir::Files);
    for (int i = 0; i < sl.size(); ++i)
    {
        BSyntax s( dir + "/" + sl.at(i) );
        if ( s.isValid() )
        {
            _m_syntaxes << s;
            _m_cmboxSyntax->addItem( s.type() );
        }
    }
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_setDocumentSyntax( _m_documents.at(i) );
}

void BTextEditor::loadTextMacros(const QString &dir, bool clear)
{
    if (clear)
        _m_cmboxTextMacros->clear();
    QDir d(dir);
    if ( !d.exists() )
        return;
    QStringList sl = d.entryList(QStringList() << "*.txt", QDir::Files);
    for (int i = 0; i < sl.size(); ++i)
    {
        QFile f( d.absoluteFilePath( sl.at(i) ) );
        if ( !f.open(QFile::ReadOnly) )
            return;
        QTextStream in(&f);
        in.setCodec("UTF-8");
        _m_cmboxTextMacros->addItem( QFileInfo( f.fileName() ).baseName(), in.readAll() );
        f.close();
    }
}

void BTextEditor::loadKeyboardLayoutsMap(const QString &fileName, const QString &codecName)
{
    _m_directMap.clear();
    _m_directUnique.clear();
    _m_reverseMap.clear();
    _m_reverseUnique.clear();
    _m_klmFileName = fileName;
    QFile f(_m_klmFileName);
    if ( !f.open(QFile::ReadOnly) )
        return _m_klmFileName.clear();
    QTextStream in(&f);
    if ( !codecName.isEmpty() )
        in.setCodec( codecName.toAscii() );
    while ( !in.atEnd() )
    {
        QStringList sl = in.readLine().split(' ', QString::SkipEmptyParts);
        if (sl.size() != 2)
            continue;
        const QString &first = sl.first();
        const QString &second = sl.last();
        if (first.length() != 1 || second.length() != 1)
            continue;
        const QChar &fc = first.at(0);
        const QChar &sc = second.at(0);
        if ( !_m_directMap.contains(fc) )
        {
            _m_directMap.insert(fc, sc);
            _m_directUnique << fc;
        }
        if ( !_m_reverseMap.contains(sc) )
        {
            _m_reverseMap.insert(sc, fc);
            _m_reverseUnique << sc;
        }
    }
    for (int i = _m_directUnique.size() - 1; i >= 0; --i)
    {
        const QChar &c = _m_directUnique.at(i);
        if ( _m_reverseUnique.contains(c) )
        {
            _m_directUnique.removeAll(c);
            _m_reverseUnique.removeAll(c);
        }
    }
}

void BTextEditor::applySettings(const QVariantMap &settings)
{
    if ( settings.contains(BTextEditorSettingsTab::IdFontFamily) )
        setFontFamily( settings.value(BTextEditorSettingsTab::IdFontFamily).toString() );
    if ( settings.contains(BTextEditorSettingsTab::IdFontPointSize) )
        setFontPointSize( settings.value(BTextEditorSettingsTab::IdFontPointSize).toInt() );
    if ( settings.contains(BTextEditorSettingsTab::IdDefaultEncoding) )
        setDefaultEncoding( settings.value(BTextEditorSettingsTab::IdDefaultEncoding).toString() );
    if ( settings.contains(BTextEditorSettingsTab::IdLineLength) )
        setLineLength( settings.value(BTextEditorSettingsTab::IdLineLength).toInt() );
    if ( settings.contains(BTextEditorSettingsTab::IdTabWidth) )
        setTabWidth( settings.value(BTextEditorSettingsTab::IdTabWidth).toInt() );
    if ( settings.contains(BTextEditorSettingsTab::IdMessageTimeout) )
        setMessageTimeout( settings.value(BTextEditorSettingsTab::IdMessageTimeout).toInt() );
}

void BTextEditor::addUpperWidget(QWidget *widget)
{
    if (!widget)
        return;
    widget->setParent(this);
    _m_vlt->insertWidget(0, widget);
}

void BTextEditor::addLowerWidget(QWidget *widget)
{
    if (!widget)
        return;
    widget->setParent(this);
    _m_vlt->addWidget(widget);
}

bool BTextEditor::askOnApplicationQuit()
{
    _m_saveSettings();
    return _m_documents.isEmpty() ? true : _m_closeAllDocuments();
}

const QString &BTextEditor::id() const
{
    return _m_CId;
}

const BTextEditor::FileTypeInfoList &BTextEditor::acceptableFileTypes() const
{
    return _m_fileTypes;
}

const QString &BTextEditor::macrosDefaultDir() const
{
    return _m_macrosDefDir;
}

const QString &BTextEditor::defaultEncoding() const
{
    return _m_codecName;
}

const QString &BTextEditor::fontFamily() const
{
    return _m_fontFamily;
}

int BTextEditor::fontPointSize() const
{
    return _m_fontPointSize;
}

int BTextEditor::lineLength() const
{
    return _m_lineLength;
}

int BTextEditor::tabWidth() const
{
    return _m_tabWidth;
}

int BTextEditor::messageTimeout() const
{
    return _m_messageTimeout;
}

const QString &BTextEditor::keyboardLayoutsMap() const
{
    return _m_klmFileName;
}

const QList<BSyntax> &BTextEditor::syntaxes() const
{
    return _m_syntaxes;
}

bool BTextEditor::isFileOpened(const QString &fileName) const
{
    if ( fileName.isEmpty() )
        return false;
    QString fn = QFileInfo(fileName).absoluteFilePath();
    for (int i = 0; i < _m_documents.size(); ++i)
        if (_m_documents.at(i)->fileName() == fn)
            return true;
    return false;
}

QStringList BTextEditor::openedFiles(const QStringList &fileNames) const
{
    QStringList sl;
    for (int i = 0; i < _m_documents.size(); ++i)
    {
        QString fn = _m_documents.at(i)->fileName();
        if ( fileNames.isEmpty() || fileNames.contains(fn, Qt::CaseInsensitive) )
            sl << fn;
    }
    return sl;
}

BAbstractSettingsTab *BTextEditor::settingsTab() const
{
    QVariantMap m;
    m.insert( BTextEditorSettingsTab::IdFontFamily, fontFamily() );
    m.insert( BTextEditorSettingsTab::IdFontPointSize, fontPointSize() );
    m.insert( BTextEditorSettingsTab::IdDefaultEncoding, defaultEncoding() );
    m.insert( BTextEditorSettingsTab::IdLineLength, lineLength() );
    m.insert( BTextEditorSettingsTab::IdTabWidth, tabWidth() );
    m.insert( BTextEditorSettingsTab::IdMessageTimeout, messageTimeout() );
    return new BTextEditorSettingsTab( m, _m_encodings.values() );
}

QToolBar *BTextEditor::toolBar(ToolBar tbarId) const
{
    return _m_toolBarMap.value(tbarId);
}

QList<QToolBar *> BTextEditor::toolBars() const
{
    return _m_toolBarMap.values();
}

QAction *BTextEditor::editorAction(Action actId) const
{
    return _m_actionMap.value(actId);
}

QList<QAction *> BTextEditor::editorActions(ToolBar tbarId) const
{
    QList<QAction *> al;
    switch (tbarId)
    {
    case NewOpenToolBar:
        al << _m_actionMap.value(NewDocumentAction);
        al << _m_actionMap.value(OpenFileAction);
        al << _m_actionMap.value(ReopenFileAction);
        al << _m_actionMap.value(RecentFilesAction);
        break;
    case SaveToolBar:
        al << _m_actionMap.value(SaveDocumentAction);
        al << _m_actionMap.value(SaveDocumentAsAction);
        al << _m_actionMap.value(SaveAllDocumentsAction);
        break;
    case CloseToolBar:
        al << _m_actionMap.value(CloseDocumentAction);
        al << _m_actionMap.value(CloseAllDocumentsAction);
        break;
    case UndoRedoToolBar:
        al << _m_actionMap.value(UndoAction);
        al << _m_actionMap.value(RedoAction);
        break;
    case ClipboardToolBar:
        al << _m_actionMap.value(CutAction);
        al << _m_actionMap.value(CopyAction);
        al << _m_actionMap.value(PasteAction);
        break;
    case DocumentToolBar:
        al << _m_actionMap.value(SwitchDocumentMain);
        al << _m_actionMap.value(MakeBookmarkAction);
        al << _m_actionMap.value(GotoNextBookmarkAction);
        al << _m_actionMap.value(SwitchBlockModeAction);
        al << _m_actionMap.value(SwitchSelectedTextLayoutAction);
        break;
    case SearchToolBar:
        al << _m_actionMap.value(FindAction);
        al << _m_actionMap.value(FindNextAction);
        al << _m_actionMap.value(ReplaceNextAction);
        break;
    case MacrosToolBar:
        al << _m_actionMap.value(RecordMacroAction);
        al << _m_actionMap.value(PlayMacroAction);
        al << _m_actionMap.value(ShowHideMacros);
        al << _m_actionMap.value(LoadMacroAction);
        al << _m_actionMap.value(SaveMacroAction);
        al << _m_actionMap.value(OpenMacrosDirAction);
    default:
        break;
    }
    return al;
}

QWidget *BTextEditor::widgetForAction(Action actId) const
{
    QAction *act = editorAction(actId);
    if (!act)
        return 0;
    switch (actId)
    {
    case NewDocumentAction:
    case OpenFileAction:
    case ReopenFileAction:
        return toolBar(NewOpenToolBar)->widgetForAction(act);
    case SaveDocumentAction:
    case SaveDocumentAsAction:
    case SaveAllDocumentsAction:
        return toolBar(SaveToolBar)->widgetForAction(act);
    case CloseDocumentAction:
    case CloseAllDocumentsAction:
        return toolBar(CloseToolBar)->widgetForAction(act);
    case UndoAction:
    case RedoAction:
        return toolBar(UndoRedoToolBar)->widgetForAction(act);
    case CutAction:
    case CopyAction:
    case PasteAction:
        return toolBar(ClipboardToolBar)->widgetForAction(act);
    case SwitchDocumentMain:
    case MakeBookmarkAction:
    case GotoNextBookmarkAction:
    case SwitchBlockModeAction:
    case SwitchSelectedTextLayoutAction:
        return toolBar(DocumentToolBar)->widgetForAction(act);
    case FindAction:
    case FindNextAction:
    case ReplaceNextAction:
        return toolBar(SearchToolBar)->widgetForAction(act);
    case RecordMacroAction:
    case PlayMacroAction:
    case LoadMacroAction:
    case SaveMacroAction:
    case OpenMacrosDirAction:
        return toolBar(MacrosToolBar)->widgetForAction(act);
    default:
        return 0;
    }
}

QWidget *BTextEditor::indicatorWidget() const
{
    return _m_wgtIndicator;
}

QMenu *BTextEditor::editorMenu(Menu mnuId) const
{
    return _m_menuMap.value(mnuId);
}

bool BTextEditor::isDocumentAvailable() const
{
    return _m_currentDocument;
}

QString BTextEditor::currentDocumentFileName() const
{
    return _m_currentDocument ? _m_currentDocument->fileName() : "";
}

QString BTextEditor::currentDocumentText() const
{
    return _m_currentDocument ? _m_currentDocument->text() : "";
}

QString BTextEditor::currentDocumentSelectedText() const
{
    return _m_currentDocument ? _m_currentDocument->selectedText() : "";
}

bool BTextEditor::isCurrentDocumentMain() const
{
    return _m_currentDocument ? _m_currentDocument->property("main").toBool() : false;
}

bool BTextEditor::hasMainDocument() const
{
    for (int i = 0; i < _m_documents.size(); ++i)
        if ( _m_documents.at(i)->property("main").toBool() )
            return true;
    return false;
}

QString BTextEditor::mainDocumentFileName() const
{
    BTextEditorDocument *doc = _m_mainDocument();
    return doc ? doc->fileName() : "";
}

QString BTextEditor::mainDocumentText() const
{
    BTextEditorDocument *doc = _m_mainDocument();
    return doc ? doc->text() : "";
}

QString BTextEditor::mainDocumentSelectedText() const
{
    BTextEditorDocument *doc = _m_mainDocument();
    return doc ? doc->selectedText() : "";
}

//

bool BTextEditor::performAction(int actId)
{
    switch (actId)
    {
    //NewOpenToolBar
    case NewDocumentAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_N, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        _m_newDocument();
        break;
    }
    case OpenFileAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_O, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        _m_openFile();
        break;
    }
    //SaveToolBar
    case SaveDocumentAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        return _m_saveDocument();
    }
    case SaveDocumentAsAction:
        return _m_saveDocumentAs();
    case SaveAllDocumentsAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier | Qt::ShiftModifier);
        _m_recorder->handleKeyPress(&ke);
        return _m_saveAllDocuments();
    }
    //CloseToolBar
    case CloseDocumentAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        return _m_closeDocument();
    }
    case CloseAllDocumentsAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier | Qt::ShiftModifier);
        _m_recorder->handleKeyPress(&ke);
        return _m_closeAllDocuments();
    }
    //UndoRedoToolBar
    case UndoAction:
        if (_m_currentDocument)
            _m_currentDocument->undo();
        break;
    case RedoAction:
        if (_m_currentDocument)
            _m_currentDocument->redo();
        break;
    //ClipboardToolBar
    case CutAction:
        if (_m_currentDocument)
            _m_currentDocument->cut();
        break;
    case CopyAction:
        if (_m_currentDocument)
            _m_currentDocument->copy();
        break;
    case PasteAction:
        if (_m_currentDocument)
            _m_currentDocument->paste();
        break;
    //DocumentToolBar
    case SwitchDocumentMain:
        _m_switchDocumentMain();
        break;
    case MakeBookmarkAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_F10, Qt::ControlModifier | Qt::ShiftModifier);
        _m_recorder->handleKeyPress(&ke);
        if (_m_currentDocument)
            _m_currentDocument->makeBookmark();
        break;
    }
    case GotoNextBookmarkAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_F10, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        if (_m_currentDocument)
            _m_currentDocument->gotoNextBookmark();
        break;
    }
    case SwitchBlockModeAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_B, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        _m_switchBlockMode();
        break;
    }
    case SwitchSelectedTextLayoutAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_L, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        if (_m_currentDocument)
            _m_currentDocument->switchSelectedTextLayout(_m_directMap, _m_reverseMap,
                                                         _m_directUnique, _m_reverseUnique);
        break;
    }
    //SearchToolBar
    case FindAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_F, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        if ( !_m_findDlg->isVisible() )
        {
            QString stext = _m_currentDocument ? _m_currentDocument->selectedText() : "";
            stext.remove( QRegExp("\\n.*") );
            if ( !stext.isEmpty() )
                _m_findDlg->setText(stext);
            _m_findDlg->show();
        }
        _m_findDlg->activateWindow();
        break;
    }
    case FindNextAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        _m_findNext();
        break;
    }
    case ReplaceNextAction:
    {
        QKeyEvent ke(QEvent::KeyPress, Qt::Key_H, Qt::ControlModifier);
        _m_recorder->handleKeyPress(&ke);
        _m_replaceNext();
        break;
    }
    //MacrosToolbar
    case RecordMacroAction:
        _m_recordMacro();
        break;
    case PlayMacroAction:
        _m_playMacro();
        break;
    case ShowHideMacros:
        _m_showHideMacros();
        break;
    case LoadMacroAction:
        return _m_loadMacro();
    case SaveMacroAction:
        return _m_saveMacro();
    case OpenMacrosDirAction:
        QDesktopServices::openUrl( QUrl::fromLocalFile( BCore::user("macros") ) );
        break;
    default:
        break;
    }
    return true;
}

void BTextEditor::reopen(const QString &codecName)
{
    if ( !_m_currentDocument || codecName.isEmpty() )
        return;
    if ( _m_currentDocument->isModified() &&
         !_m_reopenQuestion( _m_currentDocument->fileName() ) )
        return;
    if ( _m_currentDocument->reopen(codecName) )
        _m_updateEncoding(codecName);
}

void BTextEditor::newDocument(const QString &text)
{
    _m_newDocument(text);
}

void BTextEditor::openFile(const QString &fileName)
{
    openFiles( QStringList(fileName) );
}

void BTextEditor::openFiles(const QStringList &fileNames)
{
    QStringList afnl = _m_filterAcceptableFiles(fileNames);
    for (int i = 0; i < afnl.size(); ++i)
    {
        const QString &fn = afnl.at(i);
        if ( QFileInfo(fn).isFile() )
            _m_openFile(fn);
    }
}

void BTextEditor::insertText(const QString &text)
{
    if (_m_currentDocument)
    {
        _m_currentDocument->insertText(text);
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::setText(const QString &text)
{
    if (_m_currentDocument)
    {
        _m_currentDocument->setText(text);
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::deselect()
{
    if (_m_currentDocument)
    {
        _m_currentDocument->deselect();
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::saveSettings()
{
    _m_saveSettings();
}

//

void BTextEditor::changeEvent(QEvent *event)
{
    if (!event || event->type() != QEvent::LanguageChange)
        return QWidget::changeEvent(event);
    if ( !parentWidget() )
    {
        QList<QWidget *> wl = findChildren<QWidget *>();
        for (int i = 0; i < wl.size(); ++i)
            QApplication::sendEvent(wl.at(i), event);
    }
    _m_retranslateUi();
    retranslateUi();
}

void BTextEditor::retranslateUi()
{
    //
}

//

void BTextEditor::_m_init()
{
    setProperty( "help", QString("beqt/btexteditor.html") );
    _m_initSettings();
    _m_initOtherMembers();
    _m_initGui();
    installEventFilter(this); //?
    setAcceptDrops(true);
    connect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( _m_checkPasteAvailable() ) );
}

void BTextEditor::_m_initSettings()
{
    //settings
    _m_macrosDir = DefMacrosDir;
    _m_defaultEncoding = DefEncoding;
    _m_fontFamily = DefFontFamily;
    _m_fontPointSize = DefFontPointSize;
    _m_lineLength = DefLineLength;
    _m_tabWidth = DefTabWidth;
    _m_blockMode = false;
    //additional
    _m_fileDialogDir = DefFileDialogDir;
}

void BTextEditor::_m_initOtherMembers()
{
    //mappers
    _m_mapperActions = new QSignalMapper(this);
    connect( _m_mapperActions, SIGNAL( mapped(int) ), this, SLOT( performAction(int) ) );
    _m_mapperReopen = new QSignalMapper(this);
    connect( _m_mapperReopen, SIGNAL( mapped(QString) ), this, SLOT( _m_reopen(QString) ) );
    _m_mapperRecent = new QSignalMapper(this);
    connect( _m_mapperRecent, SIGNAL( mapped(QString) ), this, SLOT( openFile(QString) ) );
    //macro recorder
    _m_recorder = new BMacroRecorder(this);
    //default file type
    _m_defaultFileType = new AllFiles;
}

void BTextEditor::_m_initGui()
{
    _m_vlt = new QVBoxLayout(this);
      _m_twgt = new QTabWidget(this);
        connect(_m_twgt, SIGNAL( currentChanged(int) ), this, SLOT( _m_twgtCurrentChanged(int) ) );
      _m_vlt->addWidget(_m_twgt);
    //
    _m_initIndicator();
    _m_initToolBars();
    _m_initMenus();
    _m_initRecorderConsole();
    _m_initFindDialog();
}

void BTextEditor::_m_initIndicator()
{
    _m_wgtIndicator = new QWidget;
    QHBoxLayout *hl = new QHBoxLayout(_m_wgtIndicator);
    hl->setContentsMargins(0, 0, 0, 0);
    QFont fnt;
    fnt.setPointSize(10);
    QFont fntb = fnt;
    fntb.setBold(true);
    Qt::Alignment a = Qt::AlignVCenter | Qt::AlignRight;
    //syntax
    _m_cmboxSyntax = new QComboBox(_m_wgtIndicator);
      _m_cmboxSyntax->setEnabled(false);
      _m_cmboxSyntax->setFont(fnt);
      _m_cmboxSyntax->setFixedWidth(150);
      connect( _m_cmboxSyntax, SIGNAL( currentIndexChanged(int) ),
               this, SLOT( _m_cmboxSyntaxCurrentIndexChanged(int) ) );
      connect( this, SIGNAL( documentAvailableChanged(bool) ), _m_cmboxSyntax, SLOT( setEnabled(bool) ) );
    hl->addWidget(_m_cmboxSyntax);
    //cursor position
    _m_lblRowLabel = new QLabel(_m_wgtIndicator);
      _m_lblRowLabel->setFont(fntb);
    hl->addWidget(_m_lblRowLabel);
    _m_lblRowValue = new QLabel(_m_wgtIndicator);
      _m_lblRowValue->setFont(fnt);
      _m_lblRowValue->setAlignment(a);
      _m_lblRowValue->setFixedWidth(35);
    hl->addWidget(_m_lblRowValue);
    _m_lblColumnLabel = new QLabel(_m_wgtIndicator);
      _m_lblColumnLabel->setFont(fntb);
    hl->addWidget(_m_lblColumnLabel);
    _m_lblColumnValue = new QLabel(_m_wgtIndicator);
      _m_lblColumnValue->setFont(fnt);
      _m_lblColumnValue->setAlignment(a);
      _m_lblColumnValue->setFixedWidth(25);
    hl->addWidget(_m_lblColumnValue);
    _m_updateCursorPosition(-1, -1);
    //encoding
    _m_lblEncodingLabel = new QLabel(_m_wgtIndicator);
      _m_lblEncodingLabel->setFont(fntb);
    hl->addWidget(_m_lblEncodingLabel);
    _m_lblEncodingValue = new QLabel(_m_wgtIndicator);
      _m_lblEncodingValue->setFont(fnt);
      _m_lblEncodingValue->setAlignment(a);
    hl->addWidget(_m_lblEncodingValue);
    _m_updateEncoding(_m_codecName);
}

void BTextEditor::_m_initToolBars()
{
    QToolBar *tbar = _m_createToolBar(FileMenu, "FileToolBar");
      _m_createAction(NewDocumentAction, "filenew.png", "Ctrl+N", tbar, true);
      _m_createAction(OpenFileAction, "fileopen.png", "Ctrl+O", tbar, true);
      _m_initReopenMenu();
      _m_initRecentFilesMenu();
      tbar->addSeparator();
      _m_createAction(SaveDocumentAction, "filesave.png", "Ctrl+S", tbar);
      _m_createAction(SaveDocumentAsAction, "filesaveas.png", "", tbar);
      _m_createAction(SaveAllDocumentsAction, "save_all.png", "Ctrl+Shift+S", tbar);
      tbar->addSeparator();
      _m_createAction(CloseDocumentAction, "fileclose.png", "Ctrl+W", tbar);
      _m_createAction(CloseAllDocumentsAction, "close_all.png", "Ctrl+Shift+W", tbar);
    tbar = _m_createToolBar(EditMenu, "EditToolBar");
      _m_createAction(UndoAction, "undo.png", "Ctrl+Z", tbar);
      _m_createAction(RedoAction, "redo.png", "Ctrl+Shift+Z", tbar);
      tbar->addSeparator();
      _m_createAction(CutAction, "editcut.png", "Ctrl+X", tbar);
      _m_createAction(CopyAction, "editcopy.png", "Ctrl+C", tbar);
      _m_createAction(PasteAction, "editpaste.png", "Ctrl+V", tbar);
      tbar->addSeparator();
      _m_createAction(SwitchSelectedTextLayoutAction, "edit.png", "Ctrl+L", tbar);
      tbar->addSeparator();
      _m_createAction(FindAction, "search.png", "Ctrl+F", tbar);
      _m_createAction(FindNextAction, "find_next.png", "Ctrl+G", tbar);
      _m_createAction(ReplaceNextAction, "replace_next.png", "Ctrl+H", tbar);
    tbar = _m_createToolBar(DocumentMenu, "DocumentToolBar");
      _m_createAction(SwitchDocumentMain, "", "", tbar);
      _m_resetSwitchDocumentMainAction();
      tbar->addSeparator();
      _m_createAction(MakeBookmarkAction, "bookmark_add.png", "Ctrl+Shift+F10", tbar);
      _m_createAction(GotoNextBookmarkAction, "bookmark.png", "Ctrl+F10", tbar);
      tbar->addSeparator();
      _m_createAction(SwitchBlockModeAction, "edit.png", "Ctrl+B", tbar);
    tbar = _m_createToolBar(MacrosMenu, "MacrosToolBar");
      _m_createAction(RecordMacroAction, "", "", tbar, true);
      _m_resetRecordMacroAction();
      tbar->addSeparator();
      _m_createAction(PlayMacroAction, "player_play.png", "", tbar, true);
      _m_createAction(ShowHideMacros, ".png", "", tbar, true);
      tbar->addSeparator();
      _m_createAction(LoadMacroAction, "fileopen.png", "", tbar, true);
      _m_createAction(SaveMacroAction, "filesaveas.png", "", tbar, true);
      tbar->addSeparator();
      _m_cmboxTextMacros = new QComboBox;
        _m_cmboxTextMacros->setEnabled(false);
        _m_cmboxTextMacros->setFixedWidth(150);
        connect( _m_cmboxTextMacros, SIGNAL( activated(int) ), this, SLOT( _m_cmboxTextMacrosActivated(int) ) );
        connect( this, SIGNAL( documentAvailableChanged(bool) ), _m_cmboxTextMacros, SLOT( setEnabled(bool) ) );
      tbar->addWidget(_m_cmboxTextMacros);
}

void BTextEditor::_m_initMenus()
{
    QMenu *mnu = _m_createMenu(FileMenu, "FileMenu");
      mnu->addActions( editorActions(FileMenu) );
    mnu = _m_createMenu(EditMenu, "EditMenu");
      mnu->addActions( editorActions(EditMenu) );
    mnu = _m_createMenu(DocumentMenu, "DocumentMenu");
      mnu->addActions( editorActions(DocumentMenu) );
      mnu->setEnabled(false);
      connect( this, SIGNAL( documentAvailableChanged(bool) ), mnu, SLOT( setEnabled(bool) ) );
    mnu = _m_createMenu(MacrosMenu, "MacrosMenu");
      mnu->addActions( editorActions(MacrosMenu) );
}

void BTextEditor::_m_initReopenMenu()
{
    //TODO: add submenus and actions
    QToolBar *tbar = editorToolBar(FileMenu);
    if (!tbar)
        return;
    QMenu *mnu = new QMenu(this);
    QAction *act = mnu->menuAction();
    act->setEnabled(false);
    act->setIcon( QIcon(BCore::IcoPath + "/reload.png") );
    tbar->addAction(act);
    QToolButton *tbtn = qobject_cast<QToolButton *>( tbar->widgetForAction(act) );
    if (tbtn)
        tbtn->setPopupMode(QToolButton::InstantPopup);
    _m_actionMap.insert(ReopenFileAction, act);
    QList<BEncoding *> encl = _m_encodings.values();
    for (int i = 0; i < encl.size(); ++i)
    {
        QAction *ract = new QAction(this);
        _m_mapperReopen->setMapping( ract, encl.at(i)->codecName() );
        connect( ract, SIGNAL( triggered() ), _m_mapperReopen, SLOT( map() ) );
        mnu->addAction(ract);
    }
}

void BTextEditor::_m_initRecentFilesMenu()
{
    if (!tbar)
        tbar = toolBar(NewOpenToolBar);
    if (!tbar)
        return;
    QMenu *mnu = new QMenu(this);
    QAction *act = mnu->menuAction();
    act->setEnabled(false);
    act->setIcon( QIcon(BCore::IcoPath + "/fileopen.png") );
    tbar->addAction(act);
    QToolButton *tbtn = qobject_cast<QToolButton *>( tbar->widgetForAction(act) );
    if (tbtn)
        tbtn->setPopupMode(QToolButton::InstantPopup);
    _m_actionMap.insert(RecentFilesAction, act);
}

void BTextEditor::_m_initRecorderConsole()
{
    _m_recorderConsole = new QPlainTextEdit;
    _m_recorderConsole->setReadOnly(true);
    _m_recorderConsole->setFixedHeight(120);
    connect( _m_recorder, SIGNAL( cleared() ), _m_recorderConsole, SLOT( clear() ) );
    connect( _m_recorder, SIGNAL( keyPressAdded(QString) ), _m_recorderConsole, SLOT( appendPlainText(QString) ) );
    addUpperWidget(_m_recorderConsole);
    _m_recorderConsole->setVisible(false);
    _m_resetShowHideMacrosAction();
}

void BTextEditor::_m_initFindDialog()
{
    _m_findDlg = new BFindDialog(_m_CId, this);
    _m_findDlg->setLineLength(_m_lineLength);
    connect( _m_findDlg, SIGNAL( findNextAvailableChanged(bool) ),
             editorAction(FindNextAction), SLOT( setEnabled(bool) ) );
    connect( _m_findDlg, SIGNAL( replaceNextAvailableChanged(bool) ),
             editorAction(ReplaceNextAction), SLOT( setEnabled(bool) ) );
    connect( _m_findDlg, SIGNAL( findNext() ), this, SLOT( _m_findNext() ) );
    connect( _m_findDlg, SIGNAL( replaceNext() ), this, SLOT( _m_replaceNext() ) );
    connect( _m_findDlg, SIGNAL( replaceInSelection() ), this, SLOT( _m_replaceInSelection() ) );
    connect( _m_findDlg, SIGNAL( replaceInDocument() ), this, SLOT( _m_replaceInDocument() ) );
    connect( _m_findDlg, SIGNAL( replaceInAllDocuments() ), this, SLOT( _m_replaceInAllDocuments() ) );
}

QToolBar *BTextEditor::_m_createToolBar(int id, const QString &objectName)
{
    QToolBar *tbar = new QToolBar;
    tbar->setAllowedAreas(Qt::TopToolBarArea);
    tbar->setFloatable(false);
    tbar->setIconSize( QSize(24, 24) );
    tbar->setObjectName(objectName);
    _m_toolBarMap.insert(id, tbar);
    return tbar;
}

QMenu *BTextEditor::_m_createMenu(int id, const QString &objectName)
{
    QMenu *mnu = new QMenu;
    mnu->setObjectName(objectName);
    _m_menuMap.insert(id, mnu);
    return mnu;
}

QAction *BTextEditor::_m_createAction(int id, const QString &iconFileName,
                                      const QString &shortcut, QToolBar *tbar, bool enabled)
{
    if ( id < 0 || _m_actionMap.contains(id) )
        return 0;
    QAction *act = new QAction(this);
    act->setEnabled(enabled);
    if ( !iconFileName.isEmpty() )
        act->setIcon( QIcon(BCore::IcoPath + "/" + iconFileName) );
    if ( !shortcut.isEmpty() )
        act->setShortcut( QKeySequence(shortcut) );
    _m_mapperActions->setMapping(act, id);
    connect( act, SIGNAL( triggered() ), _m_mapperActions, SLOT( map() ) );
    _m_actionMap.insert(id, act);
    if (tbar)
        tbar->addAction(act);
    return act;
}

void BTextEditor::_m_retranslateUi()
{
    //widgets
    _m_lblRowLabel->setText( tr("Row:", "lbl text") );
    _m_lblColumnLabel->setText( tr("Column:", "lbl text") );
    _m_lblEncodingLabel->setText( tr("Encoding:", "lbl text") );
    //editor actions
    //tr("", "act WhatsThis")
    _m_retranslateAction( NewDocumentAction, tr("Create", "act text"), tr("Create new document", "act toolTip"),
                          tr("Use this action to create a new empty document with the default name",
                          "act whatsThis") );
    _m_retranslateAction( OpenFileAction, tr("Open...", "act text"), tr("Open file", "act toolTip"),
                          tr("Use this action to open an existing file", "act WhatsThis") );
    _m_retranslateAction( ReopenFileAction, "", tr("Reopen current document using encoding", "act toolTip"),
                          tr("Use this action to reopen current document using another encoding", "act WhatsThis") );
    _m_retranslateAction( RecentFilesAction, "", tr("Open recently opened files", "act toolTip"),
                          tr("Use this action to open one of the recently opened files again", "act WhatsThis") );
    _m_retranslateAction( SaveDocumentAction, tr("Save", "act text"), tr("Save current document", "act toolTip"),
                          tr("Use this action to save current document. If the corresponding file does not exist, "
                             "you will be asked to specify it\'s name", "act WhatsThis") );
    _m_retranslateAction( SaveDocumentAsAction, tr("Save as...", "act text"),
                          tr("Save current document as...", "act toolTip"),
                          tr("Use this action to save current document specifying it\'s name and encoding",
                             "act WhatsThis") );
    _m_retranslateAction( SaveAllDocumentsAction, tr("Save all", "act text"),
                          tr("Save all opened documents", "act toolTip"),
                          tr("Use this action to save all opened documents "
                             "(equivalent to using the Save action at every opened document)", "act WhatsThis") );
    _m_retranslateAction( CloseDocumentAction, tr("Close", "act text"), tr("Close current document", "act toolTip"),
                          tr("Use this action to close current document. If the document is modified, "
                             "you will be asked to save the changes", "act WhatsThis") );
    _m_retranslateAction( CloseAllDocumentsAction, tr("Close all", "act text"),
                          tr("Close all opened documents", "act toolTip"),
                          tr("Use this action to close all opened documents "
                             "(equivalent to using the Close action at every opened document)", "act WhatsThis") );
    _m_retranslateAction( UndoAction, tr("Undo", "act text"),
                          tr("Undo last action", "act toolTip") ); //TODO: whatsThis
    _m_retranslateAction( RedoAction, tr("Redo", "act text"),
                          tr("Redo undone action", "act toolTip") ); //TODO: whatsThis
    _m_retranslateAction( CutAction, tr("Cut", "act text"), tr("Cut selected text", "act toolTip"),
                          tr("Use this action to copy the text selected in current document to the clipboard, "
                             "and remove that text form the document", "act WhatsThis") );
    _m_retranslateAction( CopyAction, tr("Copy", "act text"), tr("Copy selected text", "act toolTip"),
                          tr("Use this action to copy the text selected in current document to the clipboard",
                             "act WhatsThis") );
    _m_retranslateAction( PasteAction, tr("Paste", "act text"), tr("Paste from clipboard", "act toolTip"),
                          tr("Use this action to insert the text from the clipboard into current document at "
                             "cursor position", "act WhatsThis") );
    _m_resetSwitchDocumentMainAction(); //TODO: whatsThis
    _m_retranslateAction( MakeBookmarkAction, tr("Make bookmark", "act text"),
                          tr("Make bookmark at current cursor position", "act toolTip"),
                          tr("Use this action to make a \"bookmarc\" in current document at the cursor position. "
                             "You will be able to quickly go to that bookmark later", "act WhatsThis") );
    _m_retranslateAction( GotoNextBookmarkAction, tr("Next bookmark", "act text"),
                          tr("Go to next bookmark", "act toolTip"),
                          tr("Use this action to go to one of the bookmarks made in this document "
                             "(the operation is cyclic: after you have gone to every bookmark, you will go to "
                             "the first one again)", "act WhatsThis") );
    _m_retranslateSwitchBlockModeAction(); //TODO: whatsThis
    _m_retranslateAction( SwitchSelectedTextLayoutAction, tr("Switch layout", "act text"),
                          tr("Switch selected text layout", "act toolTip"),
                          tr("This action is useful if you have entered some text in a wrong locale. "
                             "Select that text and then use this action", "act WhatsThis") );
    _m_retranslateAction( FindAction, tr("Find...", "act text"), tr("Find text...", "act toolTip"),
                          tr("Use this action to open the Find dialog. The Find dialog is used to search for "
                             "specified text or to replace one piece of text with another", "act WhatsThis") );
    _m_retranslateAction( FindNextAction, tr("Find next", "act text"), tr("Find next", "act toolTip"),
                          tr("Use this action to find next occurance of the text specified in the Find dialog",
                             "act WhatsThis") );
    _m_retranslateAction( ReplaceNextAction, tr("Replace next", "act text"), tr("Replace next", "act toolTip"),
                          tr("Use this action to replace currently selected text with the text specified in the "
                             "Find dialog, and then find next occurance of the text specified in the Find dialog",
                             "act WhatsThis") );
    _m_resetRecordMacroAction(); //TODO: whatsThis
    _m_retranslateAction( PlayMacroAction, tr("Play", "act text"), tr("Play macro", "act toolTip") ); //TODO
    _m_resetShowHideMacrosAction(); //TODO: whatsThis
    _m_retranslateAction( LoadMacroAction, tr("Load...", "act text"), tr("Load macro...", "act toolTip") );//TODO
    _m_retranslateAction( SaveMacroAction, tr("Save as...", "act text"), tr("Save macro as...", "act toolTip") );
    _m_retranslateAction( OpenMacrosDirAction, tr("Open macros dir", "act text"), tr("Open macros dir", "act text") );
    //menus
    _m_retranslateMenu( FileMenu, tr("File", "mnu title") );
    _m_retranslateMenu( EditMenu, tr("Edit", "mnu title") );
    _m_retranslateMenu( DocumentMenu, tr("Document", "mnu title") );
    _m_retranslateMenu( MacrosMenu, tr("Macros", "mnu title") );
    //menu reopen
    _m_retranslateReopenMenu();
    editorAction(RecentFilesAction)->menu()->setTitle( tr("Recent files", "mnu title") );
    //cmbox
    //_m_cmbox->setToolTip( _m_cmboxToolTip() );
    //toolBars
    _m_toolBarMap.value(SwitchDocumentToolBar)->setWindowTitle( tr("Switch document", "tbar windowTitle") );
    _m_toolBarMap.value(NewOpenToolBar)->setWindowTitle( tr("New / Open", "tbar windowTitle") );
    _m_toolBarMap.value(SaveToolBar)->setWindowTitle( tr("Save", "tbar windowTitle") );
    _m_toolBarMap.value(CloseToolBar)->setWindowTitle( tr("Close", "tbar windowTitle") );
    _m_toolBarMap.value(UndoRedoToolBar)->setWindowTitle( tr("Undo / Redo", "tbar windowTitle") );
    _m_toolBarMap.value(ClipboardToolBar)->setWindowTitle( tr("Clipboard", "tbar windowTitle") );
    _m_toolBarMap.value(DocumentToolBar)->setWindowTitle( tr("Document", "tbar windowTitle") );
    _m_toolBarMap.value(SearchToolBar)->setWindowTitle( tr("Search", "tbar windowTitle") );
    _m_toolBarMap.value(MacrosToolBar)->setWindowTitle( tr("Macros", "tbar windowTitle") );
}

void BTextEditor::_m_retranslateReopenMenu()
{
    QMenu *mnu = editorAction(ReopenFileAction)->menu();
    mnu->setTitle( tr("Reopen", "mnu toolTip") );
    QList<QAction *> al = mnu->actions();
    QList<BEncoding *> encl = _m_encodings.values();
    for (int i = 0; i < al.size() && i < encl.size(); ++i)
        al.at(i)->setText( encl.at(i)->fullName() );
}

void BTextEditor::_m_retranslateAction(Action actId, const QString &text, const QString &toolTip,
                                       const QString &whatsThis)
{
    QAction *act = _m_actionMap.value(actId);
    if (!act)
        return;
    act->setText(text);
    act->setToolTip(toolTip);
    act->setWhatsThis(whatsThis);
}

void BTextEditor::_m_retranslateMenu(Menu mnuId, const QString &title)
{
    QMenu *mnu = editorMenu(mnuId);
    if (!mnu)
        return;
    mnu->setTitle(title);
}

void BTextEditor::_m_retranslateSwitchBlockModeAction()
{
    QAction *act = editorAction(SwitchBlockModeAction);
    if (_m_blockMode)
    {
        act->setIcon( QIcon(BCore::IcoPath + "/blocks.png") );
        act->setText( tr("Mode: blocks", "act text") );
        act->setToolTip( tr("Switch to lines mode", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon(BCore::IcoPath + "/lines.png") );
        act->setText( tr("Mode: lines", "act text") );
        act->setToolTip( tr("Switch to blocks mode", "act toolTip") );
    }
}

void BTextEditor::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      _m_dir = s->value( _m_KeyDir, QDir::homePath() ).toString();
      _m_macrosDir = s->value(_m_KeyMacrosDir).toString();
      setFontFamily( s->value(_m_KeyFontFamily, _m_DefFontFamily).toString() );
      setFontPointSize( s->value( _m_KeyFontSize, QFont().pointSize() ).toInt() );
      setLineLength( s->value(_m_KeyLineLength, 120).toInt() );
      setTabWidth( s->value(_m_KeyTabWidth, 4).toInt() );
      setMessageTimeout( s->value(_m_KeyMessageTimeout, 15000).toInt() );
      setDefaultEncoding( s->value( _m_KeyDefaultCodecName, QString("UTF-8") ).toString() );
      QStringList sl = s->value(_m_KeyRecentFiles).toStringList();
      for (int i = sl.size() - 1; i >= 0; --i)
          _m_addRecentFile( sl.at(i) );
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

void BTextEditor::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->setValue(_m_KeyDir, _m_dir);
      s->setValue(_m_KeyMacrosDir, _m_macrosDir);
      s->setValue( _m_KeyFontFamily, fontFamily() );
      s->setValue( _m_KeyFontSize, fontPointSize() );
      s->setValue( _m_KeyLineLength, lineLength() );
      s->setValue( _m_KeyTabWidth, tabWidth() );
      s->setValue( _m_KeyMessageTimeout, messageTimeout() );
      s->setValue( _m_KeyDefaultCodecName, defaultEncoding() );
      QList<QAction *> al = editorAction(RecentFilesAction)->menu()->actions();
      QStringList sl;
      for (int i = 0; i < al.size(); ++i)
          sl << al.at(i)->toolTip();
      s->setValue(_m_KeyRecentFiles, sl);
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

void BTextEditor::_m_newDocument(const QString &text)
{
    QString fn;
    if ( _m_fileTypes.isEmpty() )
    {
        FileTypeInfo fti = _m_defFileType();
        fn = fti.fileName + "." + fti.suffixes.first();
    }
    else
    {
        const FileTypeInfo &fti = _m_fileTypes.first();
        const QStringList &sl = fti.suffixes;
        fn = fti.fileName + ( (sl.isEmpty() || sl.first() == "*") ? "" : ( "." + sl.first() ) );
    }
    BTextEditorDocument *doc = _m_addDocument(fn);
    doc->setText(text);
    _m_setDocumentSyntax(doc);
}

void BTextEditor::_m_openFile(const QString &fileName)
{
    QString fn = fileName;
#if defined(Q_OS_WIN)
    fn.replace('\\', '/');
#endif
    QString cn = _m_codecName;
    if ( fn.isEmpty() )
    {
        QScopedPointer<BOpenSaveDialog> osd( _m_createOpenSaveDialog(true) );
        if (!osd)
            return;
        int r = osd->exec();
        _m_handleOpenSaveDialog( osd.data() );
        if (BOpenSaveDialog::Accepted != r)
            return;
        QStringList sl = osd->selectedFiles();
        fn = sl.isEmpty() ? "" : sl.first();
        cn = osd->codecName();
    }
    if ( fn.isEmpty() )
        return;
    fn = QFileInfo(fn).absoluteFilePath();
    for (int i = 0; i < _m_documents.size(); ++i)
    {
        BTextEditorDocument *ted = _m_documents.at(i);
        if (ted->fileName() == fn)
        {
            _m_cmbox->setCurrentIndex(i);
            window()->raise();
            window()->activateWindow();
            if ( ted->isModified() )
            {
                if ( _m_reopenQuestion(fn) )
                {
                    ted->reopen(cn);
                    _m_updateEncoding( ted->codecName() );
                }
            }
            else
            {
                _m_alreadyOpenedWarning(fn);
            }
            return;
        }
    }
    bool ro = false;
    if ( isFileOpenedGlobal(fn) )
    {
        switch ( _m_reopenReadonlyQuestion(fn) )
        {
        case 2:
            ro = true;
        case 1:
            break;
        case 0:
        default:
            return;
        }
    }
    BTextEditorDocument *ted = _m_addDocument(fn);
    ted->setReadOnly( ro || checkReadOnly(fn) );
    _m_checkPasteAvailable(); //for read only documents
    _m_findDlg->setReplaceAvailable( !ted->isReadOnly() );
    ted->reopen(cn);
    _m_setDocumentSyntax(ted);
    _m_addRecentFile(fn);
    _m_updateEncoding( ted->codecName() );
    window()->raise();
    window()->activateWindow();
}

bool BTextEditor::_m_saveDocument(BTextEditorDocument *document)
{
    if (!document)
        document = _m_currentDocument;
    if (!document)
        return true;
    bool b = false;
    if ( !QFileInfo( document->fileName() ).isFile() )
        b = _m_saveDocumentAs(document);
    else
        b = document->save();
    if (!b)
        _m_failedToSaveError( document->fileName() );
    if (document == _m_currentDocument && b)
        editorAction(ReopenFileAction)->setEnabled(true);
    return b;
}

bool BTextEditor::_m_saveDocumentAs(BTextEditorDocument *document)
{
    if (!document)
        document = _m_currentDocument;
    if (!document)
        return true;
    QScopedPointer<BOpenSaveDialog> osd(
                _m_createOpenSaveDialog(false, document) );
    if (!osd)
        return true;
    int r = osd->exec();
    _m_handleOpenSaveDialog( osd.data() );
    if (BOpenSaveDialog::Accepted != r)
        return false;
    QStringList sl = osd->selectedFiles();
    QString fn = sl.isEmpty() ? "" : sl.first();
    if ( fn.isEmpty() )
        return false;
    for (int i = 0; i < _m_documents.size(); ++i)
    {
        BTextEditorDocument *ted = _m_documents.at(i);
        if (ted != _m_currentDocument && ted->fileName() == fn)
        {
            _m_cmbox->setCurrentIndex(i);
            window()->raise();
            window()->activateWindow();
            _m_alreadyOpenedWarning(fn);
            return false;
        }
    }
    QString codecName = osd->codecName();
    QString ofn = document->fileName();
    bool b = document->saveAs(fn, codecName);
    if (!b)
        _m_failedToSaveError( document->fileName() );
    if (document == _m_currentDocument && b)
        editorAction(ReopenFileAction)->setEnabled(true);
    if (b && ofn != fn)
        document->setReadOnly( checkReadOnly(fn) );
    _m_updateEncoding( document->codecName() );
    _m_addRecentFile(fn, ofn);
    if ( QFileInfo(fn).suffix() != QFileInfo(ofn).suffix() )
        _m_setDocumentSyntax(document);
    return b;
}

bool BTextEditor::_m_saveAllDocuments()
{
    for (int i = 0; i < _m_documents.size(); ++i)
    {
        BTextEditorDocument *tde = _m_documents.at(i);
        if (!tde)
            continue;
        if ( tde->isModified() )
        {
            if ( !_m_saveDocument(tde) )
                return false;
        }
        else if ( !QFileInfo( tde->fileName() ).isFile() )
        {
            if ( !_m_saveDocumentAs(tde) )
                return false;
        }
    }
    return true;
}

bool BTextEditor::_m_closeDocument(BTextEditorDocument *document)
{
    if (!document)
        document = _m_currentDocument;
    if (!document)
        return true;
    if ( document->isModified() )
    {
        switch ( _m_closeQuestion( document->fileName() ) )
        {
        case QMessageBox::Save:
            if ( !_m_saveDocument(document) )
                return false;
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
    }
    _m_slt->removeWidget( document->editWidget() );
    int index = _m_documents.indexOf(document);
    if (index < 0)
        return true;
    _m_documents.removeAll(document);
    if (document == _m_currentDocument)
        _m_currentDocument = 0;
    document->deleteLater();
    _m_cmbox->removeItem(index);
    return true;
}

bool BTextEditor::_m_closeAllDocuments()
{
    QList<BTextEditorDocument *> tedl;
    for (int i = 0; i < _m_documents.size(); ++i)
        if ( _m_documents.at(i)->isModified() )
            tedl << _m_documents.at(i);
    QScopedPointer<BSelectFilesDialog> sfd( new BSelectFilesDialog(_m_CId, tedl, this) );
    BSelectFilesDialog::Decision decision = BSelectFilesDialog::DiscardDecision;
    if ( !tedl.isEmpty() )
    {
        sfd->exec();
        decision = sfd->decision();
    }
    switch (decision)
    {
    case BSelectFilesDialog::DiscardDecision:
        _m_cmbox->blockSignals(true);
        while ( !_m_documents.isEmpty() )
        {
            BTextEditorDocument *ted = _m_documents.takeLast();
            ted->deleteLater();
        }
        _m_currentDocument = 0;
        _m_cmbox->clear();
        _m_cmbox->blockSignals(false);
        _m_cmbox->setToolTip("");
        _m_cmboxCurrentIndexChanged(-1);
        break;
    case BSelectFilesDialog::SaveDecision:
        _m_cmbox->blockSignals(true);
        tedl = sfd->selectedDocuments();
        while ( !_m_documents.isEmpty() )
        {
            BTextEditorDocument *ted = _m_documents.last();
            if ( tedl.contains(ted) )
            {
                if ( !_m_saveDocument(ted) )
                {
                    _m_cmbox->blockSignals(false);
                    _m_cmboxCurrentIndexChanged( _m_cmbox->currentIndex() );
                    return false;
                }
                int index = _m_documents.indexOf(ted);
                tedl.removeAll(ted);
                _m_cmbox->removeItem(index);
                ted->deleteLater();
                _m_documents.removeLast();
            }
        }
        _m_currentDocument = 0;
        _m_cmbox->blockSignals(false);
        _m_cmbox->setToolTip("");
        _m_cmboxCurrentIndexChanged(-1);
        break;
    case BSelectFilesDialog::CancelDecision:
        return false;
    default:
        break;
    }
    return true;
}

void BTextEditor::_m_switchDocumentMain()
{
    if (_m_documents.isEmpty() || !_m_currentDocument)
        return _m_resetSwitchDocumentMainAction();
    bool b = _m_currentDocument->property("main").toBool();
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setProperty("main", false);
    if (!b)
        _m_currentDocument->setProperty("main", true);
    _m_resetSwitchDocumentMainAction();
}

void BTextEditor::_m_switchBlockMode()
{
    _m_blockMode = !_m_blockMode;
    for (int i = 0; i < _m_documents.size(); ++i)
        _m_documents.at(i)->setBlockMode(_m_blockMode);
    _m_retranslateSwitchBlockModeAction();
}

void BTextEditor::_m_recordMacro()
{
    QAction *act = editorAction(RecordMacroAction);
    bool b = act->property("performing").toBool();
    if (b)
    {
        _m_recorder->stopRecording();
    }
    else
    {
        _m_recorder->startRecording();
        if ( !_m_recorderConsole->isVisible() )
        {
            _m_recorderConsole->setVisible(true);
            _m_resetShowHideMacrosAction();
        }
    }
    act->setProperty("performing", !b);
    _m_resetRecordMacroAction();
}

void BTextEditor::_m_playMacro()
{
    if ( editorAction(RecordMacroAction)->property("performing").toBool() )
        _m_recordMacro();
    _m_recorder->applyMacro();
}

void BTextEditor::_m_showHideMacros()
{
    _m_recorderConsole->setVisible( !_m_recorderConsole->isVisible() );
    _m_resetShowHideMacrosAction();
}

bool BTextEditor::_m_loadMacro()
{
    if ( _m_recorder->isStarted() )
        return false; //TODO
    QString fn = QFileDialog::getOpenFileName(this, tr("Select file", "getOpFN caption"),
                                              _m_macrosDir.isEmpty() ? _m_macrosDefDir : _m_macrosDir,
                                              tr("Macros", "getOpFN filter") + " (*.mcr)");
    if ( fn.isEmpty() )
        return false;
    return _m_recorder->loadMacro(fn);
}

bool BTextEditor::_m_saveMacro()
{
    if ( _m_recorder->isEmpty() )
        return false;
    QString fn = QFileDialog::getSaveFileName(this, tr("Select file name", "getSvFN caption"),
                                              _m_macrosDir.isEmpty() ? _m_macrosDefDir : _m_macrosDir,
                                              tr("Macros", "getSvFN filter") + " (*.mcr)");
    if ( fn.isEmpty() )
        return false;
    if (fn.size() < 3 || fn.right(3) != "mcr")
        fn.append(".mcr");
    return _m_recorder->saveMacro(fn);
}

void BTextEditor::_m_resetSwitchDocumentMainAction()
{
    QAction *act = editorAction(SwitchDocumentMain);
    act->setEnabled(_m_currentDocument);
    if ( _m_currentDocument && _m_currentDocument->property("main").toBool() )
    {
        act->setIcon( QIcon(BCore::IcoPath + "/edit_add.png") );
        act->setText( tr("Unset main", "act text") );
        act->setToolTip( tr("Unset main document", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon( BCore::IcoPath + (hasMainDocument() ? "/has_main_document.png" : "/edit_remove.png") ) );
        act->setText( tr("Set main", "act text") );
        act->setToolTip( tr("Set current document as main", "act toolTip") );
    }
}

void BTextEditor::_m_resetRecordMacroAction()
{
    QAction *act = editorAction(RecordMacroAction);
    if ( act->property("performing").toBool() )
    {
        act->setIcon( QIcon(BCore::IcoPath + "/player_stop.png") );
        act->setText( tr("Stop recording", "act text") );
        act->setToolTip( tr("Stop recording macro", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon(BCore::IcoPath + "/start.png") );
        act->setText( tr("Start recording", "act text") );
        act->setToolTip( tr("Start recording macro", "act toolTip") );
    }
}

void BTextEditor::_m_resetShowHideMacrosAction()
{
    QAction *act = editorAction(ShowHideMacros);
    if ( _m_recorderConsole->isVisible() )
    {
        act->setIcon( QIcon(BCore::IcoPath + "/button_cancel.png") );
        act->setText( tr("Hide", "act text") );
        act->setToolTip( tr("Hide macros console", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon(BCore::IcoPath + "/button_ok.png") );
        act->setText( tr("Show", "act text") );
        act->setToolTip( tr("Show macros console", "act toolTip") );
    }
}

BTextEditorDocument *BTextEditor::_m_addDocument(const QString &fileName)
{
    BTextEditorDocument *ted = new BTextEditorDocument(fileName, this);
    ted->setMacroRecorder(_m_recorder);
    ted->setLineLength(_m_lineLength);
    ted->setTabWidth(_m_tabWidth);
    connect( ted, SIGNAL( fileNameChanged(QString) ), this, SLOT( _m_documentFileNameChanged(QString) ) );
    connect( ted, SIGNAL( cutAvailableChanged(bool) ), editorAction(CutAction), SLOT( setEnabled(bool) ) );
    connect( ted, SIGNAL( copyAvailableChanged(bool) ), editorAction(CopyAction), SLOT( setEnabled(bool) ) );
    connect( ted, SIGNAL( undoAvailableChanged(bool) ), editorAction(UndoAction), SLOT( setEnabled(bool) ) );
    connect( ted, SIGNAL( redoAvailableChanged(bool) ), editorAction(RedoAction), SLOT( setEnabled(bool) ) );
    connect( ted, SIGNAL( modificationChanged(bool) ), this, SLOT( _m_documentModificationChanged(bool) ) );
    connect( ted, SIGNAL( hasBookmarkChanged(bool) ), editorAction(GotoNextBookmarkAction), SLOT( setEnabled(bool) ) );
    connect( ted, SIGNAL( cursorPositionChanged(int, int) ), this, SLOT( _m_updateCursorPosition(int, int) ) );
    connect( ted, SIGNAL( selectionChanged(bool) ), this, SLOT( _m_documentSelectionChanged(bool) ) );
    connect( ted, SIGNAL( requestSwitchDocument() ), this, SLOT( _m_documentSwitchRequested() ) );
    connect( ted, SIGNAL( maxLineLengthReached() ), this, SLOT( _m_maxLineLengthReached() ) );
    ted->setFontFamily(_m_fontFamily);
    ted->setFontPointSize(_m_fontPointSize);
    ted->setBlockMode(_m_blockMode);
    _m_documents << ted;
    int index = _m_slt->addWidget( ted->editWidget() );
    _m_cmbox->addItem(QFileInfo(fileName).fileName(), fileName);
    _m_cmbox->setCurrentIndex(index);
    return ted;
}

void BTextEditor::_m_addRecentFile(const QString &fileName, const QString &oldFileName)
{
    if ( fileName.isEmpty() )
        return;
    QMenu *mnu = editorAction(RecentFilesAction)->menu();
    QList<QAction *> al = mnu->actions();
    if (!oldFileName.isEmpty() && oldFileName != fileName)
    {
        for (int i = 0; i < al.size(); ++i)
        {
            QAction *oact = al.at(i);
            if (oact->toolTip() == oldFileName)
            {
                mnu->removeAction(oact);
                al.removeAt(i);
                oact->deleteLater();
                break;
            }
        }
    }
    for (int i = 0; i < al.size(); ++i)
    {
        QAction *oact = al.at(i);
        if (oact->toolTip() == fileName)
        {
            mnu->removeAction(oact);
            al.removeAt(i);
            oact->deleteLater();
            break;
        }
    }
    if (al.size() == _m_MaxRecentFiles)
    {
        QAction *oact = al.last();
        mnu->removeAction(oact);
        oact->deleteLater();
    }
    QAction *fact = !al.isEmpty() ? al.first() : 0;
    QAction *act = new QAction(this);
    act->setToolTip(fileName);
    act->setText( QFileInfo(fileName).fileName() );
    _m_mapperRecent->setMapping(act, fileName);
    connect( act, SIGNAL( triggered() ), _m_mapperRecent, SLOT( map() ) );
    mnu->insertAction(fact, act);
    editorAction(RecentFilesAction)->setEnabled(true);
}

bool BTextEditor::_m_reopenQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Reopening file", "msgbox windowTitle") );
    msg.setText(tr("Document has been modified:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to reload it? All changes will be discarded.",
                               "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec() == QMessageBox::Ok;
}

int BTextEditor::_m_reopenReadonlyQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Reopening file", "msgbox windowTitle") );
    msg.setText(tr("Document is already opened in other editor window:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to open it in this window anyway?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msg.addButton(tr("Read only mode", "btn text"), QMessageBox::DestructiveRole);
    msg.setDefaultButton(QMessageBox::Cancel);
    int ret = msg.exec();
    if (QMessageBox::Cancel == ret)
        return 0;
    else if (QMessageBox::Ok == ret)
        return 1;
    else
        return 2;
}

void BTextEditor::_m_alreadyOpenedWarning(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Warning);
    msg.setWindowTitle( tr("Reopening file", "msgbox windowTitle") );
    msg.setText(tr("File is already opened:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

int BTextEditor::_m_closeQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Saving changes", "msgbox windowTitle") );
    msg.setText(tr("Document has been modified:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to save the changes?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Discard);
    return msg.exec();
}

void BTextEditor::_m_failedToSaveError(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Critical);
    msg.setWindowTitle( tr("Failed to save file", "msgbox windowTitle") );
    msg.setText(tr("Failed to save file:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void BTextEditor::_m_resetCmboxItem(BTextEditorDocument *document, bool nameChanged)
{
    if (!document)
        return;
    int index = _m_documents.indexOf(document);
    if (index < 0)
        return;
    QString fn = document->fileName();
    _m_cmbox->setItemText( index, QFileInfo(fn).fileName() + (document->isModified() ? "*" : "") );
    if (nameChanged)
    {
        _m_cmbox->setItemData(index, fn);
        if (document == _m_currentDocument)
            _m_cmbox->setToolTip(fn);
    }
}

BTextEditor::FileTypeInfo BTextEditor::_m_defFileType() const
{
    FileTypeInfo fti;
    fti.fileName = tr("New text document", "fileTypeInfo fileName");
    fti.suffixes << "txt";
    fti.description = tr("Text files", "fileTypeInfo description");
    return fti;
}

BOpenSaveDialog *BTextEditor::_m_createOpenSaveDialog(bool openMode, BTextEditorDocument *document)
{
    if ( (document && openMode) || (!document && !openMode) )
        return 0;
    BOpenSaveDialog::Mode mode = openMode ? BOpenSaveDialog::OpenMode : BOpenSaveDialog::SaveMode;
    const QString fileName = document ? document->fileName() : "";
    QFileInfo fi(fileName);
    QString dir = ( !fileName.isEmpty() && fi.isFile() ) ? fi.path() : _m_dir;
    const QString &codecName = document ? document->codecName() : _m_codecName;
    BOpenSaveDialog *dialog  = new BOpenSaveDialog(_m_CId, mode, _m_encodings.values(), codecName, this);
    dialog->setHistory(_m_history);
    dialog->setDirectory(dir);
    dialog->setFilter( _m_fileDialogFilter() );
    QString suff = fi.suffix();
    if ( suff.isEmpty() )
        suff = !_m_fileTypes.isEmpty() ? _m_fileTypes.first().suffixes.first() : _m_defFileType().suffixes.first();
    dialog->setDefaultSuffix("*" == suff ? "" : suff);
    if ( !fileName.isEmpty() )
        dialog->selectFile(fileName);
    return dialog;
}

void BTextEditor::_m_handleOpenSaveDialog(BOpenSaveDialog *dialog)
{
    if (!dialog || dialog->result() != BOpenSaveDialog::Accepted)
        return;
    QStringList sl = dialog->selectedFiles();
    if ( !sl.isEmpty() )
        _m_dir = QFileInfo( sl.first() ).path();
    _m_history = dialog->history();
    _m_saveSettings();
}

QString BTextEditor::_m_fileDialogFilter() const
{
    FileTypeInfoList ftil = _m_fileTypes;
    if ( ftil.isEmpty() )
        ftil << _m_defFileType();
    QString filter;
    for (int i = 0; i < ftil.size(); ++i)
    {
        const FileTypeInfo &fti = ftil.at(i);
        filter += (fti.description + " (");
        const QStringList &sl = fti.suffixes;
        if ( sl.contains("*") )
        {
            filter += "*";
        }
        else
        {
            for (int j = 0; j < sl.size(); ++j)
            {
                filter += ( "*." + sl.at(j) );
                if (j < sl.size() - 1)
                    filter += " ";
            }
        }
        filter += ")";
        if (i < ftil.size() - 1)
            filter += ";;";
    }
    return filter;
}

QStringList BTextEditor::_m_filterAcceptableFiles(const QStringList &fileNames) const
{
    FileTypeInfoList ftil = _m_fileTypes;
    if ( ftil.isEmpty() )
        ftil << _m_defFileType();
    QStringList sl;
    for (int i = 0; i < ftil.size(); ++i)
        sl << ftil.at(i).suffixes;
    if ( sl.contains("*") )
        return fileNames;
    QStringList afnl;
    for (int i = 0; i < fileNames.size(); ++i)
    {
        const QString &fn = fileNames.at(i);
        if ( sl.contains( QFileInfo(fn).suffix() ) )
            afnl << fn;
    }
    return afnl;
}

void BTextEditor::_m_handleUrls(const QList<QUrl> &urls)
{
    QStringList fnl;
    for (int i = 0; i < urls.size(); ++i)
        fnl << urls.at(i).toLocalFile();
    openFiles(fnl);
}

//QString BTextEditor::_m_cmboxToolTip() const
//{
    //QString part = tr("Press <Ctrl+Tab> to switch between documents", "cmbox toolTip");
    //if (_m_currentDocument)
        //return _m_currentDocument->fileName() + " (" + part + ")";
    //else
        //return part;
//}

BTextEditorDocument *BTextEditor::_m_mainDocument() const
{
    for (int i = 0; i < _m_documents.size(); ++i)
        if ( _m_documents.at(i)->property("main").toBool() )
            return _m_documents.at(i);
    return 0;
}

void BTextEditor::_m_textReplaced(int count)
{
    emit showMessage(tr("Replaced:", "stbar message") + " " + QString::number(count), _m_messageTimeout);
}

void BTextEditor::_m_setDocumentSyntax(BTextEditorDocument *document)
{
    if ( !document || _m_syntaxes.isEmpty() )
        return;
    QString fn = document->fileName();
    bool b = false;
    for (int i = 0; i < _m_syntaxes.size(); ++i)
    {
        if ( _m_syntaxes.at(i).matchesFileName(fn) )
        {
            document->setSyntax( _m_syntaxes.at(i) );
            _m_setCmboxSyntax( _m_syntaxes.at(i) );
            b = true;
            break;
        }
    }
    if (!b)
        document->setSyntax( _m_syntaxes.first() );
}

void BTextEditor::_m_setCmboxSyntax(const BSyntax &syntax)
{
    _m_cmboxSyntax->blockSignals(true);
    _m_cmboxSyntax->setCurrentIndex( _m_cmboxSyntax->findText( syntax.type() ) );
    _m_cmboxSyntax->blockSignals(false);
}

//

void BTextEditor::_m_checkPasteAvailable()
{
    editorAction(PasteAction)->setEnabled( _m_currentDocument && !_m_currentDocument->isReadOnly() &&
                                           !QApplication::clipboard()->text().isEmpty() );
}

void BTextEditor::_m_updateCursorPosition(int row, int column)
{
    if (row >= 0)
        _m_lblRowValue->setText( QString::number(row + 1) );
    else
        _m_lblRowValue->setText("----");
    if (column >= 0)
        _m_lblColumnValue->setText( QString::number(column) );
    else
        _m_lblColumnValue->setText("---");
}

void BTextEditor::_m_updateEncoding(const QString &codecName)
{
    if ( codecName.isEmpty() )
        _m_lblEncodingValue->setText(_m_codecName);
    else
        _m_lblEncodingValue->setText(codecName);
}

/*void BTextEditor::_m_cmboxCurrentIndexChanged(int index)
{
    if ( index < 0 || index > _m_documents.size() )
        _m_currentDocument = 0;
    else
        _m_currentDocument = _m_documents.at(index);
    _m_findDlg->setDocumentAvailable(_m_currentDocument);
    if (_m_currentDocument)
        _m_findDlg->setReplaceAvailable( !_m_currentDocument->isReadOnly() );
    QString stext = _m_currentDocument ? _m_currentDocument->selectedText() : "";
    _m_documentSelectionChanged( !stext.isEmpty() );
    bool b = _m_currentDocument && QFileInfo( _m_currentDocument->fileName() ).isFile();
    editorAction(ReopenFileAction)->setEnabled(b);
    editorAction(SaveDocumentAction)->setEnabled( _m_currentDocument && _m_currentDocument->isModified() );
    editorAction(SaveDocumentAsAction)->setEnabled(_m_currentDocument);
    editorAction(SaveAllDocumentsAction)->setEnabled(_m_currentDocument);
    editorAction(CloseDocumentAction)->setEnabled(_m_currentDocument);
    editorAction(CloseAllDocumentsAction)->setEnabled(_m_currentDocument);
    editorAction(UndoAction)->setEnabled( _m_currentDocument && _m_currentDocument->isUndoAvailable() );
    editorAction(RedoAction)->setEnabled( _m_currentDocument && _m_currentDocument->isRedoAvailable() );
    editorAction(CutAction)->setEnabled( _m_currentDocument && _m_currentDocument->isCutAvailable() );
    editorAction(CopyAction)->setEnabled( _m_currentDocument && _m_currentDocument->isCopyAvailable() );
    _m_checkPasteAvailable();
    _m_resetSwitchDocumentMainAction();
    editorAction(MakeBookmarkAction)->setEnabled(_m_currentDocument);
    editorAction(GotoNextBookmarkAction)->setEnabled(_m_currentDocument && _m_currentDocument->hasBookmarks() );
    editorAction(SwitchBlockModeAction)->setEnabled(_m_currentDocument);
    editorAction(SwitchSelectedTextLayoutAction)->setEnabled( _m_currentDocument &&
                                                              _m_currentDocument->isCopyAvailable() );
    editorAction(FindAction)->setEnabled(_m_currentDocument);
    emit currentDocumentChanged(_m_currentDocument ? _m_currentDocument->fileName() : "");
    emit documentAvailableChanged(_m_currentDocument);
    _m_cmbox->setToolTip( _m_cmboxToolTip() );
    if (_m_currentDocument)
        _m_setCmboxSyntax( _m_currentDocument->syntax() );
    _m_updateEncoding(_m_currentDocument ? _m_currentDocument->codecName() : _m_codecName);
    if (!_m_currentDocument)
    {
        _m_updateCursorPosition(-1, -1);
        return;
    }
    BTextEditorDocument::CursorPosition cp = _m_currentDocument->cursorPosition();
    _m_updateCursorPosition(cp.row, cp.column);
    _m_slt->setCurrentIndex(index);
    _m_currentDocument->setFocusToEdit();
}*/

void BTextEditor::_m_twgtCurrentChanged(int index)
{
    if ( index < 0 || index > _m_documents.size() )
        _m_currentDocument = 0;
    else
        _m_currentDocument = _m_documents.at(index);
    _m_findDlg->setDocumentAvailable(_m_currentDocument);
    if (_m_currentDocument)
        _m_findDlg->setReplaceAvailable( !_m_currentDocument->isReadOnly() );
    QString stext = _m_currentDocument ? _m_currentDocument->selectedText() : "";
    _m_documentSelectionChanged( !stext.isEmpty() );
    bool b = _m_currentDocument && QFileInfo( _m_currentDocument->fileName() ).isFile();
    editorAction(ReopenFileAction)->setEnabled(b);
    editorAction(SaveDocumentAction)->setEnabled( _m_currentDocument && _m_currentDocument->isModified() );
    editorAction(SaveDocumentAsAction)->setEnabled(_m_currentDocument);
    editorAction(SaveAllDocumentsAction)->setEnabled(_m_currentDocument);
    editorAction(CloseDocumentAction)->setEnabled(_m_currentDocument);
    editorAction(CloseAllDocumentsAction)->setEnabled(_m_currentDocument);
    editorAction(UndoAction)->setEnabled( _m_currentDocument && _m_currentDocument->isUndoAvailable() );
    editorAction(RedoAction)->setEnabled( _m_currentDocument && _m_currentDocument->isRedoAvailable() );
    editorAction(CutAction)->setEnabled( _m_currentDocument && _m_currentDocument->isCutAvailable() );
    editorAction(CopyAction)->setEnabled( _m_currentDocument && _m_currentDocument->isCopyAvailable() );
    _m_checkPasteAvailable();
    _m_resetSwitchDocumentMainAction();
    editorAction(MakeBookmarkAction)->setEnabled(_m_currentDocument);
    editorAction(GotoNextBookmarkAction)->setEnabled(_m_currentDocument && _m_currentDocument->hasBookmarks() );
    editorAction(SwitchBlockModeAction)->setEnabled(_m_currentDocument);
    editorAction(SwitchSelectedTextLayoutAction)->setEnabled( _m_currentDocument &&
                                                              _m_currentDocument->isCopyAvailable() );
    editorAction(FindAction)->setEnabled(_m_currentDocument);
    emit currentDocumentChanged(_m_currentDocument ? _m_currentDocument->fileName() : "");
    emit documentAvailableChanged(_m_currentDocument);
    _m_cmbox->setToolTip( _m_cmboxToolTip() );
    if (_m_currentDocument)
        _m_setCmboxSyntax( _m_currentDocument->syntax() );
    _m_updateEncoding(_m_currentDocument ? _m_currentDocument->codecName() : _m_codecName);
    if (!_m_currentDocument)
    {
        _m_updateCursorPosition(-1, -1);
        return;
    }
    BTextEditorDocument::CursorPosition cp = _m_currentDocument->cursorPosition();
    _m_updateCursorPosition(cp.row, cp.column);
    _m_slt->setCurrentIndex(index);
    _m_currentDocument->setFocusToEdit();
}

void BTextEditor::_m_cmboxTextMacrosActivated(int index)
{
    if (index < 0)
        return;
    insertText( _m_cmboxTextMacros->itemData(index).toString() );
}

void BTextEditor::_m_cmboxSyntaxCurrentIndexChanged(int index)
{
    if ( !_m_currentDocument || index < 0 || index >= _m_syntaxes.size() )
        return;
    _m_currentDocument->setSyntax( _m_syntaxes.at(index) );
    _m_currentDocument->setFocusToEdit();
}

void BTextEditor::_m_documentSwitchRequested()
{
    if (!_m_currentDocument || _m_documents.size() < 2)
        return;
    int ind = _m_documents.indexOf(_m_currentDocument);
    if (ind < 0)
        return;
    if (ind < _m_documents.size() - 1)
        _m_cmbox->setCurrentIndex(ind + 1);
    else
        _m_cmbox->setCurrentIndex(0);
}

void BTextEditor::_m_documentFileNameChanged(const QString &fileName)
{
    _m_resetCmboxItem(qobject_cast<BTextEditorDocument *>( sender() ), true);
}

void BTextEditor::_m_documentModificationChanged(bool modified)
{
    _m_resetCmboxItem( qobject_cast<BTextEditorDocument *>( sender() ) );
    editorAction(SaveDocumentAction)->setEnabled(modified);
}

void BTextEditor::_m_documentSelectionChanged(bool hasSelection)
{
    editorAction(SwitchSelectedTextLayoutAction)->setEnabled(_m_currentDocument &&
                                                             !_m_currentDocument->isReadOnly() && hasSelection);
    _m_findDlg->setSelectionAvailable(hasSelection);
}

void BTextEditor::_m_findNext()
{
    if (!_m_currentDocument)
        return;
    QString text = _m_findDlg->text();
    if ( !_m_currentDocument->find( text, _m_findDlg->findFlags(), _m_findDlg->cyclic() ) )
        emit showMessage(tr("Could not find", "stbar message") + " \"" + text + "\"", _m_messageTimeout);
}

void BTextEditor::_m_replaceNext()
{
    if ( !_m_currentDocument || _m_currentDocument->isReadOnly() )
        return;
    if ( _m_currentDocument->replace( _m_findDlg->newText() ) )
        _m_findNext();
}

void BTextEditor::_m_replaceInSelection()
{
    if ( !_m_currentDocument || _m_currentDocument->isReadOnly() )
        return;
    QString text = _m_findDlg->text();
    QString newText = _m_findDlg->newText();
    Qt::CaseSensitivity cs = _m_findDlg->caseSensitivity();
    int count = _m_currentDocument->replaceInSelection(text, newText, cs);
    if (count >= 0)
        _m_textReplaced(count);
}

void BTextEditor::_m_replaceInDocument()
{
    if ( !_m_currentDocument || _m_currentDocument->isReadOnly() )
        return;
    QString text = _m_findDlg->text();
    QString newText = _m_findDlg->newText();
    Qt::CaseSensitivity cs = _m_findDlg->caseSensitivity();
    int count = _m_currentDocument->replaceInDocument(text, newText, cs);
    if (count >= 0)
        _m_textReplaced(count);
}

void BTextEditor::_m_replaceInAllDocuments()
{
    if ( _m_documents.isEmpty() )
        return;
    QString text = _m_findDlg->text();
    QString newText = _m_findDlg->newText();
    Qt::CaseSensitivity cs = _m_findDlg->caseSensitivity();
    int count = 0;
    bool b = true;
    for (int i = 0; i < _m_documents.size(); ++i)
    {
        BTextEditorDocument *ted = _m_documents.at(i);
        if ( ted->isReadOnly() )
            continue;
        int c = ted->replaceInDocument(text, newText, cs);
        if (c >= 0)
            count += c;
        else
            b = false;
    }
    if (b)
        _m_textReplaced(count);
}

void BTextEditor::_m_maxLineLengthReached()
{
    emit showMessage(tr("Maximum line length reached!", "stbar message"), _m_messageTimeout);
}
