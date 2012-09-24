#include "btexteditor.h"
#include "bsyntax.h"
#include "babstractsettingstab.h"
#include "babstractfiletype.h"

#include "private/btexteditordocument.h"
#include "private/bselectfilesdialog.h"
#include "private/bfinddialog.h"
#include "private/bopensavedialog.h"
#include "private/btexteditorsettingstab.h"
#include "private/bmacrorecorder.h"
#include "private/bplaintextedit.h"
#include "private/ballfiles.h"

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
#include <QRect>

#include <QDebug>

BTextEditor::SettingsOptions::SettingsOptions()
{
    toolBarIconSize = true;
    fontFamily = true;
    fontPointSize = true;
    defaultEncoding = true;
    lineLength = true;
    tabWidth = true;
    keyboardLayoutMap = true;
    macrosDir = true;
}

////

const QString BTextEditor::_m_SyntaxTypeDef = "Text";
const QString BTextEditor::_m_MacrosDirDef = QDir::homePath();
//
const QRect BTextEditor::_m_OpenSaveDlgGeometryDef = QRect(200, 200, 400, 400);
const QString BTextEditor::_m_OpenSaveDlgDirDef = QDir::homePath();
//
const QRect BTextEditor::_m_SelectFilesDlgGeometryDef = QRect(200, 200, 600, 400);
//
const int BTextEditor::_m_MessageTimeout = 15 * BCore::Second;
const int BTextEditor::_m_RecentFilesMax = 15;
//
const QString BTextEditor::_m_GroupTextEditor = "beqt_text_editor";
  const QString BTextEditor::_m_KeyMacrosDir = "macros_dir";
  const QString BTextEditor::_m_KeyToolBarIconSize = "toolbar_icon_size";
  const QString BTextEditor::_m_KeyDefaultEncoding = "default_encoding";
  const QString BTextEditor::_m_KeyFontFamily = "font_family";
  const QString BTextEditor::_m_KeyFontPointSize = "font_point_size";
  const QString BTextEditor::_m_KeyLineLength = "line_length";
  const QString BTextEditor::_m_KeyTabWidth = "tab_width";
  const QString BTextEditor::_m_KeyKeyboardLayoutMap = "keyboard_layout_map";
  const QString BTextEditor::_m_KeyRecentFiles = "recent_files";
  const QString BTextEditor::_m_GroupFindDialog = "find_dialog";
    const QString BTextEditor::_m_KeyTextHistory = "text_history";
    const QString BTextEditor::_m_KeyNewTextHistory = "new_text_history";
    const QString BTextEditor::_m_GroupOptions = "options";
      const QString BTextEditor::_m_KeyCaseSensitive = "case_sensitive";
      const QString BTextEditor::_m_KeyWholeWords = "whole_words";
      const QString BTextEditor::_m_KeyBackwardOrder = "backward_order";
      const QString BTextEditor::_m_KeyCyclic = "cyclic";
  const QString BTextEditor::_m_GroupOpenSaveDialog = "open_save_dialog";
    const QString BTextEditor::_m_KeyGeometry = "geometry";
    const QString BTextEditor::_m_KeyDir = "dir";
  const QString BTextEditor::_m_GroupSelectFilesDialog = "select_files_dialog";
    //_m_KeyDir

//

QList<BTextEditor *> BTextEditor::_m_instanceList;

//

QList<BTextEditor *> BTextEditor::_m_instances(const QString &groupId)
{
    QList<BTextEditor *> list;
    for (int i = 0; i < _m_instanceList.size(); ++i)
        if (_m_instanceList.at(i)->groupId() == groupId)
            list << _m_instanceList.at(i);
    return list;
}

BTextEditorDocument *BTextEditor::_m_document(QWidget *widget)
{
    BPlainTextEdit *pted = qobject_cast<BPlainTextEdit *>(widget);
    return pted ? pted->editorDocument() : 0;
}

bool BTextEditor::_m_checkReadOnly(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    QFile f(fileName);
    bool ro = !f.open(QFile::WriteOnly | QFile::Append);
    f.close();
    return ro;
}

QAction *BTextEditor::_m_createSeparator()
{
    QAction *act = new QAction(0);
    act->setSeparator(true);
    return act;
}

//

bool BTextEditor::isFileOpenedGlobal(const QString &fileName, const QString &groupId)
{
    QList<BTextEditor *> list = _m_instances(groupId);
    for (int i = 0; i < list.size(); ++i)
        if ( list.at(i)->isFileOpened(fileName) )
            return true;
    return false;
}

//

const QString BTextEditor::SettingsTabId = "beqt/text_editor";

//

BTextEditor::BTextEditor(QWidget *parent, bool registerGlobally, const QString &groupId) :
    QWidget(parent), _m_CRegistered(registerGlobally), _m_CGroupId(groupId)
{
    if (registerGlobally)
        _m_instanceList << this;
    _m_init();
    _m_retranslateUi();
    connect( BCore::instance(), SIGNAL( localeChanged() ), this, SLOT( _m_retranslateUi() ) );
}

BTextEditor::~BTextEditor()
{
    if (_m_CRegistered)
        _m_instanceList.removeAll(this);
    if (_m_wgtIndicator)
        _m_wgtIndicator->deleteLater();
    for (int i = 0; i < _m_userFileTypes.size(); ++i)
        _m_userFileTypes.at(i)->deleteLater();
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
    {
        QList<QUrl> urls = static_cast<QDropEvent *>(event)->mimeData()->urls();
        QStringList fnl;
        for (int i = 0; i < urls.size(); ++i)
            fnl << urls.at(i).toLocalFile();
        openFiles(fnl);
        return true;
    }
    default:
        break;
    }
    return QWidget::eventFilter(object, event);
}

//settings:set

void BTextEditor::setUserFileTypes(QList<BAbstractFileType *> list)
{
    for (int i = 0; i < _m_userFileTypes.size(); ++i)
        _m_userFileTypes.at(i)->deleteLater();
    _m_userFileTypes = list;
    _m_cmboxSyntax->blockSignals(true);
    _m_cmboxSyntax->clear();
    _m_cmboxSyntax->addItem("Text");
    for (int i = 0; i < list.size(); ++i)
        _m_cmboxSyntax->addItem( list.at(i)->syntax().type() );
    _m_cmboxSyntax->blockSignals(false);
}

void BTextEditor::setDefaultFileName(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return;
    _m_defaultFileName = fileName;
}

void BTextEditor::setDefaultMacrosDir(const QString &dir)
{
    if ( !QDir(dir).exists() )
        return;
    _m_defaultMacrosDir = dir;
}

void BTextEditor::setToolBarIconSize(int size)
{
    if (!BTextEditorSettingsTab::checkToolBarIconSize(size) || size == _m_toolBarIconSize)
        return;
    _m_toolBarIconSize = size;
    QList<QToolBar *> list = _m_toolBars.values();
    for (int i = 0; i < list.size(); ++i)
        list.at(i)->setIconSize( QSize(size, size) );
}

void BTextEditor::setMacrosDir(const QString &dir)
{
    if ( dir.isEmpty() )
        return;
    _m_macrosDir = dir;
}

void BTextEditor::setDefaultEncoding(const QString &codecName)
{
    if (!BTextEditorDocument::checkEncoding(codecName) || codecName == _m_defaultEncoding)
        return;
    _m_defaultEncoding = codecName;
    loadTextMacros( textMacrosDirs() );
    if ( _m_currentDocument.isNull() )
        _m_updateEncoding(_m_defaultEncoding);
}

void BTextEditor::setFontFamily(const QString &family)
{
    if (!BTextEditorDocument::checkFontFamily(family) || family == _m_fontFamily)
        return;
    _m_fontFamily = family;
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setFontFamily(_m_fontFamily);
}

void BTextEditor::setFontPointSize(int pointSize)
{
    if (!BTextEditorDocument::checkFontPointSize(pointSize) || pointSize == _m_fontPointSize)
        return;
    _m_fontPointSize = pointSize;
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setFontPointSize(_m_fontPointSize);
}

void BTextEditor::setLineLength(int length)
{
    if (!BTextEditorDocument::checkLineLength(length) || length == _m_lineLength)
        return;
    _m_lineLength = length;
    _m_findDlg->setLineLength(_m_lineLength);
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setLineLength(_m_lineLength);
}

void BTextEditor::setTabWidth(int width)
{
    if (!BTextEditorDocument::checkTabWidth(width) || width == _m_tabWidth)
        return;
    _m_tabWidth = width;
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setTabWidth(_m_tabWidth);
}

void BTextEditor::setKeyboardLayoutMap(const QString &description)
{
    if ( description.isEmpty() )
        return;
    _m_currentKeyboardLayoutMap = description;
    _m_documentSelectionChanged( !_m_currentDocument.isNull() && _m_currentDocument->hasSelection() );

}

void BTextEditor::setBlockMode(bool enabled)
{
    if (enabled == _m_blockMode)
        return;
    _m_switchBlockMode();
}

void BTextEditor::setFindDialogSettings(const FindDialogSettings &settings)
{
    BFindDialog::Parameters param;
    param.backwardOrder = settings.backwardOrder;
    param.caseSensitive = settings.caseSensitive;
    param.cyclic = settings.cyclic;
    param.newTextHistory = settings.newTextHistory;
    param.textHistory = settings.textHistory;
    param.wholeWords = settings.wholeWords;
    _m_findDlg->setParameters(param);
}

void BTextEditor::applySettings(const QVariantMap &settings)
{
    //macros dir
    if ( settings.contains(BTextEditorSettingsTab::IdMacrosDir) )
        setMacrosDir( settings.value(BTextEditorSettingsTab::IdMacrosDir).toString() );
    if ( settings.contains(BTextEditorSettingsTab::IdToolBarIconSize) )
        setToolBarIconSize( settings.value(BTextEditorSettingsTab::IdToolBarIconSize).toInt() );
    //default encoding
    if ( settings.contains(BTextEditorSettingsTab::IdDefaultEncoding) )
        setDefaultEncoding( settings.value(BTextEditorSettingsTab::IdDefaultEncoding).toString() );
    //font family
    if ( settings.contains(BTextEditorSettingsTab::IdFontFamily) )
        setFontFamily( settings.value(BTextEditorSettingsTab::IdFontFamily).toString() );
    //font point size
    if ( settings.contains(BTextEditorSettingsTab::IdFontPointSize) )
        setFontPointSize( settings.value(BTextEditorSettingsTab::IdFontPointSize).toInt() );
    //line length
    if ( settings.contains(BTextEditorSettingsTab::IdLineLength) )
        setLineLength( settings.value(BTextEditorSettingsTab::IdLineLength).toInt() );
    //tab width
    if ( settings.contains(BTextEditorSettingsTab::IdTabWidth) )
        setTabWidth( settings.value(BTextEditorSettingsTab::IdTabWidth).toInt() );
    //keyboard layout map
    if ( settings.contains(BTextEditorSettingsTab::IdKeyboardLayoutMap) )
        setKeyboardLayoutMap( settings.value(BTextEditorSettingsTab::IdKeyboardLayoutMap).toString() );
    //applying to other instances
    if (_m_CRegistered)
    {
        QList<BTextEditor *> list = _m_instances(_m_CGroupId);
        list.removeAll(this);
        for (int i = 0; i < list.size(); ++i)
            list.at(i)->applySettings(settings);
    }
}

//settings:get

bool BTextEditor::isRegisteredGlobally() const
{
    return _m_CRegistered;
}

const QString &BTextEditor::groupId() const
{
    return _m_CGroupId;
}

QList<BAbstractFileType *> BTextEditor::userFileTypes() const
{
    QList<BAbstractFileType *> list;
    for (int i = 0; i < _m_userFileTypes.size(); ++i)
        list << _m_userFileTypes.at(i)->clone();
    return list;
}

const QString &BTextEditor::defaultFileName() const
{
    return _m_defaultFileName;
}

const QString &BTextEditor::macrosDir() const
{
    return _m_macrosDir;
}

int BTextEditor::toolBarIconSize() const
{
    return _m_toolBarIconSize;
}

const QString &BTextEditor::defaultEncoding() const
{
    return _m_defaultEncoding;
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

const QString &BTextEditor::keyboardLayoutMap() const
{
    return _m_currentKeyboardLayoutMap;
}

bool BTextEditor::blockMode() const
{
    return _m_blockMode;
}

BTextEditor::FindDialogSettings BTextEditor::findDialogSettings() const
{
    BFindDialog::Parameters param = _m_findDlg->parameters();
    FindDialogSettings s;
    s.backwardOrder = param.backwardOrder;
    s.caseSensitive = param.caseSensitive;
    s.cyclic = param.cyclic;
    s.newTextHistory = param.newTextHistory;
    s.textHistory = param.textHistory;
    s.wholeWords = param.wholeWords;
    return s;
}

const QString &BTextEditor::lastOpenSaveDialogDir() const
{
    return _m_openSaveDlgDir;
}

BAbstractSettingsTab *BTextEditor::createSettingsTab(const SettingsOptions &opt) const
{
    QVariantMap m;
    if (opt.macrosDir)
        m.insert( BTextEditorSettingsTab::IdMacrosDir, macrosDir() );
    if (opt.toolBarIconSize)
        m.insert( BTextEditorSettingsTab::IdToolBarIconSize, toolBarIconSize() );
    if (opt.defaultEncoding)
        m.insert( BTextEditorSettingsTab::IdDefaultEncoding, defaultEncoding() );
    if (opt.fontFamily)
        m.insert( BTextEditorSettingsTab::IdFontFamily, fontFamily() );
    if (opt.fontPointSize)
        m.insert( BTextEditorSettingsTab::IdFontPointSize, fontPointSize() );
    if (opt.lineLength)
        m.insert( BTextEditorSettingsTab::IdLineLength, lineLength() );
    if (opt.tabWidth)
        m.insert( BTextEditorSettingsTab::IdTabWidth, tabWidth() );
    if (opt.keyboardLayoutMap)
        m.insert( BTextEditorSettingsTab::IdKeyboardLayoutMap, keyboardLayoutMap() );
    return !m.isEmpty() ? new BTextEditorSettingsTab( m, _m_encodingsMap(), _m_keyboardLayoutMaps.keys() ) : 0;
}

//settings:load/save

//loading/saving settings

void BTextEditor::loadSettings(const QString &settingsGroup)
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !settingsGroup.isEmpty() )
        s->beginGroup(settingsGroup);
    s->beginGroup(_m_GroupTextEditor);
      setMacrosDir( s->value(_m_KeyMacrosDir, _m_defaultMacrosDir).toString() );
      setToolBarIconSize( s->value(_m_KeyToolBarIconSize, BTextEditorSettingsTab::ToolBarIconSizeDef).toInt() );
      setDefaultEncoding( s->value(_m_KeyDefaultEncoding, BTextEditorDocument::EncodingDef).toString() );
      setFontFamily( s->value(_m_KeyFontFamily, BTextEditorDocument::FontFamilyDef).toString() );
      setFontPointSize( s->value(_m_KeyFontPointSize, BTextEditorDocument::FontPointSizeDef).toInt() );
      setLineLength( s->value(_m_KeyLineLength, BTextEditorDocument::LineLengthDef).toInt() );
      setTabWidth( s->value(_m_KeyTabWidth, BTextEditorDocument::TabWidthDef).toInt() );
      setKeyboardLayoutMap( s->value(_m_KeyKeyboardLayoutMap).toString() );
      _m_loadRecentFiles( s->value(_m_KeyRecentFiles).toStringList() );
      s->beginGroup(_m_GroupFindDialog);
        BFindDialog::Parameters param;
        param.textHistory = s->value(_m_KeyTextHistory).toStringList();
        param.newTextHistory = s->value(_m_KeyNewTextHistory).toStringList();
        s->beginGroup(_m_GroupOptions);
          param.caseSensitive = s->value(_m_KeyCaseSensitive).toBool();
          param.wholeWords = s->value(_m_KeyWholeWords).toBool();
          param.backwardOrder = s->value(_m_KeyBackwardOrder).toBool();
          param.cyclic = s->value(_m_KeyCyclic, true).toBool();
        s->endGroup();
        _m_findDlg->setParameters(param);
      s->endGroup();
      s->beginGroup(_m_GroupOpenSaveDialog);
        _m_openSaveDlgGeometry = s->value(_m_KeyGeometry, _m_OpenSaveDlgGeometryDef).toRect();
        _m_openSaveDlgDir = s->value(_m_KeyDir, _m_OpenSaveDlgDirDef).toString();
      s->endGroup();
      s->beginGroup(_m_GroupSelectFilesDialog);
        _m_selectFilesDlgGeometry = s->value(_m_KeyGeometry, _m_SelectFilesDlgGeometryDef).toRect();
      s->endGroup();
    s->endGroup();
    if ( !settingsGroup.isEmpty() )
        s->endGroup();
}

void BTextEditor::saveSettings(const QString &settingsGroup)
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !settingsGroup.isEmpty() )
        s->beginGroup(settingsGroup);
    s->beginGroup(_m_GroupTextEditor);
      s->setValue(_m_KeyMacrosDir, _m_macrosDir);
      s->setValue(_m_KeyToolBarIconSize, _m_toolBarIconSize);
      s->setValue( _m_KeyDefaultEncoding, defaultEncoding() );
      s->setValue( _m_KeyFontFamily, fontFamily() );
      s->setValue( _m_KeyFontPointSize, fontPointSize() );
      s->setValue( _m_KeyLineLength, lineLength() );
      s->setValue( _m_KeyTabWidth, tabWidth() );
      s->setValue( _m_KeyKeyboardLayoutMap, keyboardLayoutMap() );
      s->setValue( _m_KeyRecentFiles, _m_saveRecentFiles() );
      s->beginGroup(_m_GroupFindDialog);
        BFindDialog::Parameters param = _m_findDlg->parameters();
        s->setValue(_m_KeyTextHistory, param.textHistory);
        s->setValue(_m_KeyNewTextHistory, param.newTextHistory);
        s->beginGroup(_m_GroupOptions);
          s->setValue(_m_KeyCaseSensitive, param.caseSensitive);
          s->setValue(_m_KeyWholeWords, param.wholeWords);
          s->setValue(_m_KeyBackwardOrder, param.backwardOrder);
          s->setValue(_m_KeyCyclic, param.cyclic);
        s->endGroup();
      s->endGroup();
      s->beginGroup(_m_GroupOpenSaveDialog);
        s->setValue(_m_KeyGeometry, _m_openSaveDlgGeometry);
        s->setValue(_m_KeyDir, _m_openSaveDlgDir);
      s->endGroup();
      s->beginGroup(_m_GroupSelectFilesDialog);
        s->setValue(_m_KeyGeometry, _m_selectFilesDlgGeometry);
      s->endGroup();
    s->endGroup();
    if ( !settingsGroup.isEmpty() )
        s->endGroup();
}

//loadable content

void BTextEditor::loadTextMacros(const QString &dir)
{
    loadTextMacros(QStringList() << dir);
}

void BTextEditor::loadTextMacros(const QStringList &dirs)
{
    _m_textMacrosDirs = dirs;
    QAction *act = editorAction(TextMacrosAction);
    QMenu *mnu = act->menu();
    mnu->clear();
    QStringList sl;
    for (int i = 0; i < dirs.size(); ++i)
    {
        const QString &dir = dirs.at(i);
        QStringList files = QDir(dir).entryList(QStringList() << "*.txt", QDir::Files);
        for (int j = 0; j < files.size(); ++j)
        {
            QFile f( dir + "/" + files.at(j) );
            if ( sl.contains( f.fileName() ) )
                continue;
            if ( !f.open(QFile::ReadOnly) )
                continue;
            QTextStream in(&f);
            in.setCodec("UTF-8");
            QString text = in.readAll();
            f.close();
            if ( text.isEmpty() )
                continue;
            sl << f.fileName();
            QAction *mact = mnu->addAction( QFileInfo( files.at(j) ).baseName() );
            _m_mapperTextMacros->setMapping(mact, text);
            connect( mact, SIGNAL( triggered() ), _m_mapperTextMacros, SLOT( map() ) );
        }
    }
}

void BTextEditor::loadKeyboardLayoutMaps(const QString &dir)
{
    loadKeyboardLayoutMaps(QStringList() << dir);
}

void BTextEditor::loadKeyboardLayoutMaps(const QStringList &dirs)
{
    _m_keyboardLayoutMapsDirs = dirs;
    _m_keyboardLayoutMaps.clear();
    for (int i = 0; i < dirs.size(); ++i)
    {
        const QString &dir = dirs.at(i);
        QStringList files = QDir(dir).entryList(QStringList() << "*.klm", QDir::Files);
        for (int j = 0; j < files.size(); ++j)
        {
            BKeyboardLayoutMap klm( dir + "/" + files.at(i) );
            if ( !klm.isValid() )
                continue;
            _m_keyboardLayoutMaps.insert(klm.description(), klm);
        }
    }
}

const QStringList &BTextEditor::textMacrosDirs() const
{
    return _m_textMacrosDirs;
}

const QStringList &BTextEditor::keyboardLayoutMapsDirs() const
{
    return _m_keyboardLayoutMapsDirs;
}

//gui:set

void BTextEditor::setTabsClosable(bool closable)
{
    _m_twgt->setTabsClosable(closable);
}

void BTextEditor::setTabsMovable(bool movable)
{
    _m_twgt->setMovable(movable);
}

void BTextEditor::addWidget(QWidget *widget)
{
    if (!widget)
        return;
    widget->setParent(this);
    _m_vlt->addWidget(widget);
}

void BTextEditor::insertWidget(int index, QWidget *widget)
{
    if (!widget)
        return;
    widget->setParent(this);
    _m_vlt->insertWidget(index, widget);
}

//gui:get

QToolBar *BTextEditor::editorToolBar(Menu id) const
{
    return _m_toolBars.value(id);
}

QList<QToolBar *> BTextEditor::editorToolBars() const
{
    return _m_toolBars.values();
}

QMenu *BTextEditor::editorMenu(Menu id) const
{
    return _m_menus.value(id);
}

QList<QMenu *> BTextEditor::editorMenus() const
{
    return _m_menus.values();
}

QAction *BTextEditor::editorAction(Action id) const
{
    return _m_actions.value(id);
}

QList<QAction *> BTextEditor::editorActions(Menu id, bool includeSeparators) const
{
    QList<QAction *> list;
    switch (id)
    {
    case FileMenu:
        list << _m_actions.value(NewDocumentAction);
        list << _m_actions.value(OpenFileAction);
        list << _m_actions.value(ReopenFileAction);
        list << _m_actions.value(RecentFilesAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(SaveDocumentAction);
        list << _m_actions.value(SaveDocumentAsAction);
        list << _m_actions.value(SaveAllDocumentsAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(CloseDocumentAction);
        list << _m_actions.value(CloseAllDocumentsAction);
        break;
    case EditMenu:
        list << _m_actions.value(UndoAction);
        list << _m_actions.value(RedoAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(CutAction);
        list << _m_actions.value(CopyAction);
        list << _m_actions.value(PasteAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(SwitchSelectedTextLayoutAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(FindAction);
        list << _m_actions.value(FindNextAction);
        break;
    case DocumentMenu:
        list << _m_actions.value(SwitchDocumentMain);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(MakeBookmarkAction);
        list << _m_actions.value(GotoNextBookmarkAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(SwitchBlockModeAction);
        break;
    case MacrosMenu:
        list << _m_actions.value(RecordMacroAction);
        list << _m_actions.value(ClearMacroAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(PlayMacroAction);
        list << _m_actions.value(ShowHideMacrosConsole);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(LoadMacroAction);
        list << _m_actions.value(SaveMacroAction);
        if (includeSeparators)
            list << _m_createSeparator();
        list << _m_actions.value(TextMacrosAction);
        break;
    default:
        break;
    }
    return list;
}

QList<QAction *> BTextEditor::editorActions() const
{
    return _m_actions.values();
}

QWidget *BTextEditor::indicatorWidget() const
{
    return _m_wgtIndicator;
}

QWidget *BTextEditor::widget(int index) const
{
    return _m_vlt->itemAt(index)->widget();
}

//document

bool BTextEditor::isDocumentAvailable() const
{
    return !_m_currentDocument.isNull();
}

bool BTextEditor::hasMainDocument() const
{
    return _m_mainDocument();
}

bool BTextEditor::isCurrentDocumentMain() const
{
    return !_m_currentDocument.isNull() && _m_currentDocument->property("main").toBool();
}

QString BTextEditor::currentDocumentFileName() const
{
    return !_m_currentDocument.isNull() ? _m_currentDocument->fileName() : "";
}

QString BTextEditor::currentDocumentText() const
{
    return !_m_currentDocument.isNull() ? _m_currentDocument->text() : "";
}

QString BTextEditor::currentDocumentSelectedText() const
{
    return !_m_currentDocument.isNull() ? _m_currentDocument->selectedText() : "";
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

//other

bool BTextEditor::askOnClose()
{
    return _m_currentDocument.isNull() || _m_closeAllDocuments();
}

bool BTextEditor::isFileOpened(const QString &fileName) const
{
    if ( fileName.isEmpty() )
        return false;
    QString fn = QFileInfo(fileName).absoluteFilePath();
    for (int i = 0; i < _m_twgt->count(); ++i)
        if (_m_document(i)->fileName() == fn)
            return true;
    return false;
}

QStringList BTextEditor::openedFiles(const QStringList &fileNames) const
{
    QStringList sl;
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        QString fn = _m_document(i)->fileName();
        if ( fileNames.isEmpty() || fileNames.contains(fn, Qt::CaseInsensitive) )
            sl << fn;
    }
    return sl;
}

//

bool BTextEditor::performAction(int id)
{
    switch (id)
    {
    //FileMenu
    case NewDocumentAction:
        _m_newDocument();
        break;
    case OpenFileAction:
        return _m_openFile();
    case SaveDocumentAction:
        return _m_saveDocument();
    case SaveDocumentAsAction:
        return _m_saveDocumentAs();
    case SaveAllDocumentsAction:
        return _m_saveAllDocuments();
    case CloseDocumentAction:
        return _m_closeDocument();
    case CloseAllDocumentsAction:
        return _m_closeAllDocuments();
    //EditMenu
    case UndoAction:
        _m_undo();
        break;
    case RedoAction:
        _m_redo();
        break;
    case CutAction:
        _m_cut();
        break;
    case CopyAction:
        _m_copy();
        break;
    case PasteAction:
        _m_paste();
        break;
    case SwitchSelectedTextLayoutAction:
        _m_switchSelectedTextLayout();
        break;
    case FindAction:
        _m_find();
        break;
    case FindNextAction:
        _m_findNext();
        break;
    //DocumentMenu
    case SwitchDocumentMain:
        _m_switchDocumentMain();
        break;
    case MakeBookmarkAction:
        _m_makeBookmark();
        break;
    case GotoNextBookmarkAction:
        _m_gotoNextBookmark();
        break;
    case SwitchBlockModeAction:
        _m_switchBlockMode();
        break;
    //MacrosMenu
    case RecordMacroAction:
        _m_recordMacro();
        break;
    case PlayMacroAction:
        _m_playMacro();
        break;
    case ShowHideMacrosConsole:
        _m_showHideMacrosConsole();
        break;
    case LoadMacroAction:
        return _m_loadMacro();
    case SaveMacroAction:
        return _m_saveMacro();
    default:
        break;
    }
    return true;
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
    for (int i = 0; i < fileNames.size(); ++i)
    {
        const QString &fn = fileNames.at(i);
        if ( QFileInfo(fn).exists() )
            _m_openFile(fn);
    }
}

void BTextEditor::insertText(const QString &text)
{
    if ( !_m_currentDocument.isNull() )
    {
        _m_currentDocument->insertText(text);
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::setText(const QString &text)
{
    if ( !_m_currentDocument.isNull() )
    {
        _m_currentDocument->setText(text);
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::deselect()
{
    if ( !_m_currentDocument.isNull() )
    {
        _m_currentDocument->deselect();
        _m_currentDocument->setFocusToEdit();
    }
}

void BTextEditor::setFocusToEdit()
{
    if ( _m_currentDocument.isNull() )
        return;
    _m_currentDocument->setFocusToEdit();
}

//init:main

void BTextEditor::_m_init()
{
    _m_initSettings();
    _m_initOtherMembers();
    _m_initGui();
    installEventFilter(this);
    setAcceptDrops(true);
    connect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( _m_clipboardDataChanged() ) );
}

void BTextEditor::_m_initSettings()
{
    //settings
    _m_defaultFileName = tr("New document", "document fileName");
    _m_defaultMacrosDir = _m_MacrosDirDef;
    _m_toolBarIconSize = BTextEditorSettingsTab::ToolBarIconSizeDef;
    _m_defaultEncoding = BTextEditorDocument::EncodingDef;
    _m_fontFamily = BTextEditorDocument::FontFamilyDef;
    _m_fontPointSize = BTextEditorDocument::FontPointSizeDef;
    _m_lineLength = BTextEditorDocument::LineLengthDef;
    _m_tabWidth = BTextEditorDocument::TabWidthDef;
    _m_blockMode = BTextEditorDocument::BlockModeDef;
    //additional
    _m_openSaveDlgGeometry = _m_OpenSaveDlgGeometryDef;
    _m_openSaveDlgDir = _m_OpenSaveDlgDirDef;
    _m_selectFilesDlgGeometry = _m_SelectFilesDlgGeometryDef;
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
    _m_mapperTextMacros = new QSignalMapper(this);
    connect( _m_mapperTextMacros, SIGNAL( mapped(QString) ), this, SLOT( insertText(QString) ) );
    //macro recorder
    _m_recorder = new BMacroRecorder(this);
    //default file type
    _m_defaultFileType = new BAllFiles(this);
}

void BTextEditor::_m_initGui()
{
    _m_vlt = new QVBoxLayout(this);
      _m_vlt->setContentsMargins(0, 0, 0, 0);
      _m_twgt = new QTabWidget(this);
        _m_twgt->setMovable(true);
        _m_twgt->setTabsClosable(true);
        connect( _m_twgt, SIGNAL( currentChanged(int) ), this, SLOT( _m_twgtCurrentChanged(int) ) );
        connect( _m_twgt, SIGNAL( tabCloseRequested(int) ), this, SLOT( _m_twgtTabCloseRequested(int) ) );
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
    fnt.setFamily(BTextEditorDocument::FontFamilyDef);
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
      _m_lblColumnValue->setFixedWidth(35);
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
    _m_updateEncoding(_m_defaultEncoding);
}

void BTextEditor::_m_initToolBars()
{
    QToolBar *tbar = _m_createToolBar(FileMenu, "FileToolBar");
      _m_createAction(NewDocumentAction, tbar, "filenew", "Ctrl+N", true);
      _m_createAction(OpenFileAction, tbar, "fileopen", "Ctrl+O", true);
      _m_initReopenMenu();
      _m_createMenuAction(RecentFilesAction, 0, "history", false);
      tbar->addSeparator();
      _m_createAction(SaveDocumentAction, tbar, "filesave", "Ctrl+S");
      _m_createAction(SaveDocumentAsAction, tbar, "filesaveas");
      _m_createAction(SaveAllDocumentsAction, 0, "save_all", "Ctrl+Shift+S");
      tbar->addSeparator();
      _m_createAction(CloseDocumentAction, tbar, "fileclose", "Ctrl+W");
      _m_createAction(CloseAllDocumentsAction, 0, "close_all", "Ctrl+Shift+W");
    tbar = _m_createToolBar(EditMenu, "EditToolBar");
      _m_createAction(UndoAction, tbar, "edit_undo", "Ctrl+Z");
      _m_createAction(RedoAction, tbar, "edit_redo", "Ctrl+Shift+Z");
      tbar->addSeparator();
      _m_createAction(CutAction, tbar, "editcut", "Ctrl+X");
      _m_createAction(CopyAction, tbar, "editcopy", "Ctrl+C");
      _m_createAction(PasteAction, tbar, "editpaste", "Ctrl+V");
      tbar->addSeparator();
      _m_createAction(SwitchSelectedTextLayoutAction, 0, "charset", "Ctrl+L");
      _m_createAction(FindAction, tbar, "edit_find", "Ctrl+F");
      _m_createAction(FindNextAction, tbar, "edit_find_next", "Ctrl+G");
    tbar = _m_createToolBar(DocumentMenu, "DocumentToolBar");
      _m_createAction(SwitchDocumentMain, tbar, "");
      _m_resetSwitchDocumentMainAction();
      tbar->addSeparator();
      _m_createAction(MakeBookmarkAction, tbar, "bookmark_add", "Ctrl+Shift+F10");
      _m_createAction(GotoNextBookmarkAction, tbar, "bookmark", "Ctrl+F10");
      tbar->addSeparator();
      _m_createAction(SwitchBlockModeAction, tbar, "text_left", "Ctrl+B");
    tbar = _m_createToolBar(MacrosMenu, "MacrosToolBar");
      _m_createAction(RecordMacroAction, tbar, "", "", true);
      _m_resetRecordMacroAction();
      QAction *act = _m_createAction(ClearMacroAction, 0, "editclear");
      connect( _m_recorder, SIGNAL( macroAvailableChanged(bool) ), act, SLOT( setEnabled(bool) ) );
      connect( act, SIGNAL( triggered() ), _m_recorder, SLOT( clear() ) );
      tbar->addSeparator();
      act = _m_createAction(PlayMacroAction, tbar, "player_play");
      connect( _m_recorder, SIGNAL( macroAvailableChanged(bool) ), act, SLOT( setEnabled(bool) ) );
      _m_createAction(ShowHideMacrosConsole, tbar, "console_visible", "", true);
      _m_createAction(LoadMacroAction, 0, "fileopen", "", true);
      act = _m_createAction(SaveMacroAction, 0, "filesaveas");
      connect( _m_recorder, SIGNAL( macroAvailableChanged(bool) ), act, SLOT( setEnabled(bool) ) );
      tbar->addSeparator();
      _m_createMenuAction(TextMacrosAction, tbar, "editpaste");
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
    QMenu *mnu = _m_createMenuAction(ReopenFileAction, 0, "reload")->menu();
      _m_mnuReopenUnicode = new QMenu(this);
        _m_mnuReopenUnicode->addActions( _m_createReopenActions(QStringList() << "UTF-16" << "UTF-8") );
      mnu->addMenu(_m_mnuReopenUnicode);
      _m_mnuReopenEasternEurope = new QMenu(this);
        _m_mnuReopenEasternEurope->addActions( _m_createReopenActions(QStringList() << "ISO 8859-13" << "ISO 8859-4"
            << "Windows-1257" << "ISO 8859-6" << "KOI8-R" << "Windows-1251"
            << "KOI8-U" << "ISO 8859-16" << "ISO 8859-2" << "Windows-1250") );
      mnu->addMenu(_m_mnuReopenEasternEurope);
      _m_mnuReopenWesternEurope = new QMenu(this);
        _m_mnuReopenWesternEurope->addActions( _m_createReopenActions(QStringList() << "ISO 8859-7" << "Windows-1253"
            << "IBM 850" << "ISO 8859-1" << "ISO 8859-15" << "Apple Roman"
            << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10" << "ISO 8859-3") );
      mnu->addMenu(_m_mnuReopenWesternEurope);
      _m_mnuReopenEastAsia = new QMenu(this);
        _m_mnuReopenEastAsia->addActions( _m_createReopenActions(QStringList() << "Windows-1258" << "Big5"
            << "Big5-HKSCS" << "GB18030-0" << "EUC-KR" << "JOHAB" << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS") );
      mnu->addMenu(_m_mnuReopenEastAsia);
      _m_mnuReopenSouthEastSouthWestAsia = new QMenu(this);
        _m_mnuReopenSouthEastSouthWestAsia->addActions( _m_createReopenActions(QStringList() << "TIS-620"
                                                                               << "ISO 8859-9" << "Windows-1254") );
      mnu->addMenu(_m_mnuReopenSouthEastSouthWestAsia);
      _m_mnuReopenMiddleEast = new QMenu(this);
        _m_mnuReopenMiddleEast->addActions( _m_createReopenActions(QStringList() << "ISO 8859-6" << "Windows-1256"
                                                                   << "Windows-1255" << "ISO 8859-8") );
      mnu->addMenu(_m_mnuReopenMiddleEast);
}

void BTextEditor::_m_initRecorderConsole()
{
    _m_recorderConsole = new QPlainTextEdit;
    _m_recorderConsole->setReadOnly(true);
    _m_recorderConsole->setFixedHeight(120);
    connect( _m_recorder, SIGNAL( cleared() ), _m_recorderConsole, SLOT( clear() ) );
    connect( _m_recorder, SIGNAL( keyPressAdded(QString) ), _m_recorderConsole, SLOT( appendPlainText(QString) ) );
    insertWidget(0, _m_recorderConsole);
    _m_recorderConsole->setVisible(false);
    _m_resetShowHideMacrosAction();
}

void BTextEditor::_m_initFindDialog()
{
    _m_findDlg = new BFindDialog(this);
    _m_findDlg->setLineLength(_m_lineLength);
    connect( _m_findDlg, SIGNAL( findNextAvailableChanged(bool) ),
             editorAction(FindNextAction), SLOT( setEnabled(bool) ) );
    connect( _m_findDlg, SIGNAL( findNext() ), this, SLOT( _m_findNext() ) );
    connect( _m_findDlg, SIGNAL( replaceNext() ), this, SLOT( _m_replaceNext() ) );
    connect( _m_findDlg, SIGNAL( replaceInSelection() ), this, SLOT( _m_replaceInSelection() ) );
    connect( _m_findDlg, SIGNAL( replaceInDocument() ), this, SLOT( _m_replaceInDocument() ) );
    connect( _m_findDlg, SIGNAL( replaceInAllDocuments() ), this, SLOT( _m_replaceInAllDocuments() ) );
}

//retranslate

void BTextEditor::_m_retranslateReopenMenu()
{
    //submenus
    _m_mnuReopenUnicode->setTitle( tr("Unicode", "mnu title") );
    _m_mnuReopenEasternEurope->setTitle( tr("Eastern Europe", "mnu title") );
    _m_mnuReopenWesternEurope->setTitle( tr("Western Europe", "mnu title") );
    _m_mnuReopenEastAsia->setTitle( tr("East Asia", "mnu title") );
    _m_mnuReopenSouthEastSouthWestAsia->setTitle( tr("South-East and South-West Asia", "mnu title") );
    _m_mnuReopenMiddleEast->setTitle( tr("Middle East", "mnu title") );
    //actions
    _m_setReopenActionsText( QStringList() << "UTF-16" << "UTF-8", tr("Unicode", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257",
                             tr("Baltic", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-5" << "KOI8-R" << "Windows-1251", tr("Cyrillic", "act text") );
    _m_setReopenActionsText( QStringList() << "KOI8-U", tr("Cyrillic (Ukrainian)", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-16", tr("Romanic", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-2" << "Windows-1250", tr("Central European", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-7" << "Windows-1253", tr("Greek", "act text") );
    _m_setReopenActionsText( QStringList() << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
                             << "Apple Roman" << "Windows-1252", tr("Western", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-14", tr("Celtic", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-10", tr("Nordic", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-3", tr("South European", "act text") );
    _m_setReopenActionsText( QStringList() << "Windows-1258", tr("Vietnamese", "act text") );
    _m_setReopenActionsText( QStringList() << "Big5" << "Big5-HKSCS", tr("Traditional Chinese", "act text") );
    _m_setReopenActionsText( QStringList() << "GB18030-0", tr("Simplified Chinese", "act text") );
    _m_setReopenActionsText( QStringList() << "EUC-KR" << "JOHAB", tr("Korean", "act text") );
    _m_setReopenActionsText( QStringList() << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS", tr("Japanese", "act text") );
    _m_setReopenActionsText( QStringList() << "TIS-620", tr("Thai", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-9" << "Windows-1254", tr("Turkish", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-6" << "Windows-1256", tr("Arabic", "act text") );
    _m_setReopenActionsText( QStringList() << "Windows-1255", tr("Hebrew", "act text") );
    _m_setReopenActionsText( QStringList() << "ISO 8859-8", tr("Hebrew (visual)", "act text") );

}

void BTextEditor::_m_retranslateAction(Action id, const QString &text, const QString &toolTip,
                                       const QString &whatsThis)
{
    QAction *act = _m_actions.value(id);
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

void BTextEditor::_m_setReopenActionsText(const QStringList &codecNames, const QString &description)
{
    for (int i = 0; i < codecNames.size(); ++i)
    {
        const QString &cn = codecNames.at(i);
        if ( cn.isEmpty() )
            continue;
        QAction *act = _m_reopenActions.value(cn);
        if (!act)
            continue;
        QString text = cn;
        if ( !description.isEmpty() )
            text.prepend(description + " (").append(")");
        act->setText(text);
    }
}

void BTextEditor::_m_retranslateSwitchBlockModeAction()
{
    QAction *act = editorAction(SwitchBlockModeAction);
    if (_m_blockMode)
    {
        act->setIcon( QIcon( BCore::beqtIcon("mode_blocks") ) );
        act->setText( tr("Mode: blocks", "act text") );
        act->setToolTip( tr("Switch to lines mode", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon( BCore::beqtIcon("mode_lines") ) );
        act->setText( tr("Mode: lines", "act text") );
        act->setToolTip( tr("Switch to blocks mode", "act toolTip") );
    }
}

//actions:file

void BTextEditor::_m_newDocument(const QString &text)
{
    BTextEditorDocument *ted = _m_addDocument(_m_defaultFileName);
    ted->setText(text);
    _m_twgt->setCurrentWidget( ted->editWidget() );
}

bool BTextEditor::_m_openFile(const QString &fileName)
{
    QString fn = fileName;
#if defined(Q_OS_WIN)
    fn.replace('\\', '/');
#endif
    QString cn = _m_defaultEncoding;
    if ( fn.isEmpty() )
    {
        QScopedPointer<BOpenSaveDialog> osd( _m_createOpenSaveDialog(true) );
        if (!osd)
            return false;
        int r = osd->exec();
        _m_handleOpenSaveDialog( osd.data() );
        if (BOpenSaveDialog::Accepted != r)
            return false;
        QStringList sl = osd->selectedFiles();
        if ( sl.isEmpty() )
            return false;
        fn = sl.first();
        cn = osd->codecName();
    }
    else
    {
        fn = QFileInfo(fn).absoluteFilePath();
    }
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        BTextEditorDocument *ted = _m_document(i);
        if (ted->fileName() == fn)
        {
            _m_twgt->setCurrentIndex(i);
            window()->raise();
            window()->activateWindow();
            if ( ted->isModified() )
            {
                if ( _m_reloadModifiedQuestion(fn) )
                {
                    ted->reopen(cn); //TODO: handle possible error
                    _m_updateEncoding( ted->codecName() );
                }
            }
            else
            {
                _m_alreadyOpenedInformation(fn);
            }
            return true;
        }
    }
    bool ro = false;
    if ( _m_CRegistered && isFileOpenedGlobal(fn, _m_CGroupId) )
    {
        switch ( _m_openMultipleQuestion(fn) )
        {
        case _m_OpenReadonly:
            ro = true;
            break;
        case _m_Open:
            break;
        case _m_CancelOpening:
        default:
            return false;
        }
    }
    BTextEditorDocument *ted = _m_addDocument(fn);
    ted->setReadOnly( ro || _m_checkReadOnly(fn) );
    //TODO: maybe checking paste action is needed
    _m_findDlg->setReplaceAvailable( !ted->isReadOnly() );
    ted->reopen(cn);
    _m_addRecentFile(fn);
    _m_updateEncoding( ted->codecName() );
    window()->raise();
    window()->activateWindow();
    _m_twgt->setCurrentWidget( ted->editWidget() );
    return true;
}

bool BTextEditor::_m_saveDocument(BTextEditorDocument *document)
{
    if (!document)
        document = _m_currentDocument;
    if (!document)
        return true;
    bool b = false;
    if ( !QFileInfo( document->fileName() ).exists() )
    {
        b = _m_saveDocumentAs(document);
    }
    else
    {
        b = document->save();
        if (!b)
            _m_saveFailureError( document->fileName() );
    }
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
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        BTextEditorDocument *ted = _m_document(i);
        if (ted != _m_currentDocument && ted->fileName() == fn)
        {
            _m_twgt->setCurrentIndex(i);
            window()->raise();
            window()->activateWindow();
            _m_alreadyOpenedInformation(fn);
            return false;
        }
    }
    QString codecName = osd->codecName();
    QString ofn = document->fileName();
    bool b = document->saveAs(fn, codecName);
    if (!b)
        _m_saveFailureError( document->fileName() );
    if (document == _m_currentDocument && b)
        editorAction(ReopenFileAction)->setEnabled(true);
    if (b && ofn != fn)
        document->setReadOnly( _m_checkReadOnly(fn) );
    _m_updateEncoding( document->codecName() );
    _m_addRecentFile(fn, ofn);
    if ( QFileInfo(fn).suffix() != QFileInfo(ofn).suffix() )
        _m_autoselectDocumentSyntax(document);
    return b;
}

bool BTextEditor::_m_saveAllDocuments()
{
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        BTextEditorDocument *tde = _m_document(i);
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
        switch ( _m_closeModifiedQuestion( document->fileName() ) )
        {
        case _m_CloseSave:
            if ( !_m_saveDocument(document) )
                return false;
            break;
        case _m_CloseDiscard:
            break;
        case _m_CancelClosing:
            return false;
        default:
            break;
        }
    }
    _m_twgt->removeTab( _m_twgt->indexOf( document->editWidget() ) );
    if (document == _m_currentDocument)
        _m_currentDocument = 0;
    document->deleteLater();
    return true;
}

bool BTextEditor::_m_closeAllDocuments()
{
    QList<BTextEditorDocument *> tedl;
    for (int i = 0; i < _m_twgt->count(); ++i)
        if ( _m_document(i)->isModified() )
            tedl << _m_document(i);
    QScopedPointer<BSelectFilesDialog> sfd( new BSelectFilesDialog(tedl, this) );
    sfd->setGeometry(_m_selectFilesDlgGeometry);
    BSelectFilesDialog::Decision decision = BSelectFilesDialog::DiscardDecision;
    if ( !tedl.isEmpty() )
    {
        sfd->exec();
        decision = sfd->decision();
        _m_selectFilesDlgGeometry = sfd->geometry();
    }
    switch (decision)
    {
    case BSelectFilesDialog::SaveDecision:
        tedl = sfd->selectedDocuments();
        for (int i = 0; i < tedl.size(); ++i)
        {
            if ( !_m_saveDocument( tedl.at(i) ) )
                return false;
        }
    case BSelectFilesDialog::DiscardDecision:
    {
        _m_twgt->blockSignals(true);
        QList<BTextEditorDocument *> documents;
        for (int i = 0; i < _m_twgt->count(); ++i)
            documents << _m_document(i);
        for (int i = 0; i < documents.size(); ++i)
            documents.at(i)->deleteLater();
        _m_twgt->clear();
        _m_currentDocument = 0;
        _m_twgt->blockSignals(false);
        _m_twgtCurrentChanged(-1);
        break;
    }
    case BSelectFilesDialog::CancelDecision:
        return false;
    default:
        break;
    }
    return true;
}

//actions:edit

void BTextEditor::_m_undo()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->undo();
}

void BTextEditor::_m_redo()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->redo();
}

void BTextEditor::_m_cut()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->cut();
}

void BTextEditor::_m_copy()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->copy();
}

void BTextEditor::_m_paste()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->paste();
}

void BTextEditor::_m_switchSelectedTextLayout()
{
    if ( _m_currentDocument.isNull() || !_m_keyboardLayoutMaps.contains(_m_currentKeyboardLayoutMap) )
        return;
    _m_currentDocument->switchSelectedTextLayout( _m_keyboardLayoutMaps.value(_m_currentKeyboardLayoutMap) );
}

void BTextEditor::_m_find()
{
    if ( !_m_findDlg->isVisible() )
    {
        QString stext = _m_currentDocument ? _m_currentDocument->selectedText() : "";
        stext.remove( QRegExp("\\n.*") );
        if ( !stext.isEmpty() )
            _m_findDlg->setText(stext);
        _m_findDlg->show();
    }
    _m_findDlg->activateWindow();
}

//actions:document

void BTextEditor::_m_switchDocumentMain()
{
    if ( _m_currentDocument.isNull() )
        return _m_resetSwitchDocumentMainAction();
    bool b = _m_currentDocument->property("main").toBool();
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setProperty("main", false);
    if (!b)
        _m_currentDocument->setProperty("main", true);
    _m_resetSwitchDocumentMainAction();
}

void BTextEditor::_m_makeBookmark()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->makeBookmark();
}

void BTextEditor::_m_gotoNextBookmark()
{
    if ( !_m_currentDocument.isNull() )
        _m_currentDocument->gotoNextBookmark();
}

void BTextEditor::_m_switchBlockMode()
{
    _m_blockMode = !_m_blockMode;
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setBlockMode(_m_blockMode);
    _m_retranslateSwitchBlockModeAction();
}

//actions:macros

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
    editorAction(LoadMacroAction)->setEnabled(b);
    _m_resetRecordMacroAction();
}

void BTextEditor::_m_playMacro()
{
    if ( editorAction(RecordMacroAction)->property("performing").toBool() )
        _m_recordMacro();
    _m_recorder->applyMacro();
}

void BTextEditor::_m_showHideMacrosConsole()
{
    _m_recorderConsole->setVisible( !_m_recorderConsole->isVisible() );
    _m_resetShowHideMacrosAction();
}

bool BTextEditor::_m_loadMacro()
{
    if ( _m_recorder->isStarted() )
        return false; //TODO
    QString fn = QFileDialog::getOpenFileName(this, tr("Select file", "getOpFN caption"),
                                              !_m_macrosDir.isEmpty() ? _m_macrosDir : _m_defaultMacrosDir,
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
                                              !_m_macrosDir.isEmpty() ? _m_macrosDir : _m_defaultMacrosDir,
                                              tr("Macros", "getSvFN filter") + " (*.mcr)");
    if ( fn.isEmpty() )
        return false;
    if (fn.size() < 3 || fn.right(3) != "mcr")
        fn.append(".mcr");
    return _m_recorder->saveMacro(fn);
}

//tools:main

BTextEditorDocument *BTextEditor::_m_addDocument(const QString &fileName)
{
    BTextEditorDocument *ted = new BTextEditorDocument(fileName, _m_defaultEncoding, this);
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
    connect( ted, SIGNAL( maxLineLengthReached() ), this, SLOT( _m_documentMaxLineLengthReached() ) );
    ted->setFontFamily(_m_fontFamily);
    ted->setFontPointSize(_m_fontPointSize);
    ted->setBlockMode(_m_blockMode);
    _m_autoselectDocumentSyntax(ted);
    int ind = _m_twgt->addTab( ted->editWidget(), QFileInfo(fileName).fileName() );
    _m_twgt->setTabToolTip(ind, fileName);
    return ted;
}

BTextEditorDocument *BTextEditor::_m_document(int index) const
{
    return _m_document( _m_twgt->widget(index) );
}

BTextEditorDocument *BTextEditor::_m_mainDocument() const
{
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        BTextEditorDocument *ted = _m_document(i);
        if ( ted->property("main").toBool() )
            return ted;
    }
    return 0;
}

void BTextEditor::_m_loadRecentFiles(const QStringList &fileNames)
{
    for (int i = fileNames.size() - 1; i >= 0; --i)
        _m_addRecentFile( fileNames.at(i) );
}

QStringList BTextEditor::_m_saveRecentFiles() const
{
    QStringList list;
    QList<QAction *> acts = editorAction(RecentFilesAction)->menu()->actions();
    for (int i = 0; i < acts.size(); ++i)
        list << acts.at(i)->toolTip();
    return list;
}

void BTextEditor::_m_addRecentFile(const QString &fileName, const QString &oldFileName)
{
    //TODO: improve
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
    if (al.size() == _m_RecentFilesMax)
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

BOpenSaveDialog *BTextEditor::_m_createOpenSaveDialog(bool openMode, BTextEditorDocument *document)
{
    if ( (document && openMode) || (!document && !openMode) )
        return 0;
    BOpenSaveDialog::Mode mode = openMode ? BOpenSaveDialog::OpenMode : BOpenSaveDialog::SaveMode;
    const QString fileName = document ? document->fileName() : "";
    QFileInfo fi(fileName);
    QString dir = ( !fileName.isEmpty() && fi.isFile() ) ? fi.path() : _m_openSaveDlgDir;
    const QString &codecName = document ? document->codecName() : _m_defaultEncoding;
    BOpenSaveDialog *dialog  = new BOpenSaveDialog(mode, _m_encodingsMap(), codecName, this);
    dialog->setHistory(_m_openSaveDlgHistory);
    dialog->setDirectory(dir);
    QString filter;
    for (int i = 0; i < _m_userFileTypes.size(); ++i)
        filter += _m_userFileTypes.at(i)->fileDialogFilter() + ";;";
    filter += _m_defaultFileType->fileDialogFilter();
    dialog->setFilter(filter);
    dialog->setGeometry(_m_openSaveDlgGeometry);
    dialog->setDefaultSuffix( fi.suffix() );
    if ( !fileName.isEmpty() )
        dialog->selectFile(fileName);
    return dialog;
}

void BTextEditor::_m_handleOpenSaveDialog(BOpenSaveDialog *dialog)
{
    if (!dialog)
        return;
    _m_openSaveDlgHistory = dialog->history();
    _m_openSaveDlgGeometry = dialog->geometry();
    if (dialog->result() != BOpenSaveDialog::Accepted)
        return;
    QStringList sl = dialog->selectedFiles();
    if ( !sl.isEmpty() )
        _m_openSaveDlgDir = QFileInfo( sl.first() ).path();
}

//tools:messages

void BTextEditor::_m_alreadyOpenedInformation(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle( tr("Information", "msgbox windowTitle") );
    msg.setText(tr("File is already opened:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

bool BTextEditor::_m_reloadModifiedQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Question", "msgbox windowTitle") );
    msg.setText(tr("Document has been modified:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to reload it? All changes will be discarded.",
                               "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec() == QMessageBox::Ok;
}

BTextEditor::_m_CloseModifiedQuestionResult BTextEditor::_m_closeModifiedQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Question", "msgbox windowTitle") );
    msg.setText(tr("Document has been modified:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to save the changes?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Discard);
    switch ( msg.exec() )
    {
    case QMessageBox::Discard:
        return _m_CloseDiscard;
    case QMessageBox::Save:
        return _m_CloseSave;
    default:
        return _m_CancelClosing;
    }
}

BTextEditor::_m_OpenMultipleQuestionResult BTextEditor::_m_openMultipleQuestion(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Question);
    msg.setWindowTitle( tr("Question", "msgbox windowTitle") );
    msg.setText(tr("Document is already opened in other editor window:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setInformativeText( tr("Do you want to open it in this window anyway?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msg.addButton(tr("Read only mode", "btn text"), QMessageBox::DestructiveRole);
    msg.setDefaultButton(QMessageBox::Cancel);
    switch ( msg.exec() )
    {
    case QMessageBox::Ok:
        return _m_Open;
    case QMessageBox::Cancel:
        return _m_CancelOpening;
    default:
        return _m_OpenReadonly;
    }
}

void BTextEditor::_m_saveFailureError(const QString &fileName)
{
    QMessageBox msg(this);
    msg.setIcon(QMessageBox::Critical);
    msg.setWindowTitle( tr("Error", "msgbox windowTitle") );
    msg.setText(tr("Failed to save file:", "msgbox text") + " <i>" + fileName + "</i>");
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

//tools:create

QToolBar *BTextEditor::_m_createToolBar(Menu id, const QString &objectName)
{
    if ( _m_toolBars.contains(id) )
        return 0;
    QToolBar *tbar = new QToolBar;
    tbar->setFloatable(false);
    tbar->setIconSize( QSize(_m_toolBarIconSize, _m_toolBarIconSize) );
    tbar->setObjectName(objectName);
    _m_toolBars.insert(id, tbar);
    return tbar;
}

QMenu *BTextEditor::_m_createMenu(Menu id, const QString &objectName)
{
    if ( _m_menus.contains(id) )
        return 0;
    QMenu *mnu = new QMenu;
    mnu->setObjectName(objectName);
    _m_menus.insert(id, mnu);
    return mnu;
}

QAction *BTextEditor::_m_createAction(Action id, QToolBar *tbar, const QString &iconName,
                                      const QString &shortcut, bool enabled)
{
    if ( _m_actions.contains(id) )
        return 0;
    QAction *act = new QAction(this);
    act->setEnabled(enabled);
    act->setIcon( QIcon( BCore::beqtIcon(iconName) ) );
    if ( !shortcut.isEmpty() )
        act->setShortcut( QKeySequence(shortcut) );
    _m_mapperActions->setMapping(act, id);
    connect( act, SIGNAL( triggered() ), _m_mapperActions, SLOT( map() ) );
    _m_actions.insert(id, act);
    if (tbar)
        tbar->addAction(act);
    return act;
}

QAction *BTextEditor::_m_createMenuAction(Action id, QToolBar *tbar, const QString &iconName, bool enabled)
{
    if ( _m_actions.contains(id) )
        return 0;
    QMenu *mnu = new QMenu(this);
    QAction *act = mnu->menuAction();
    act->setEnabled(enabled);
    act->setIcon( QIcon( BCore::beqtIcon(iconName) ) );
    if (tbar)
    {
        tbar->addAction(act);
        qobject_cast<QToolButton *>( tbar->widgetForAction(act) )->setPopupMode(QToolButton::InstantPopup);
    }
    _m_actions.insert(id, act);
    return act;
}

QList<QAction *> BTextEditor::_m_createReopenActions(const QStringList &codecNames)
{
    QList<QAction *> acts;
    for (int i = 0; i < codecNames.size(); ++i)
    {
        const QString &cn = codecNames.at(i);
        if ( cn.isEmpty() || _m_reopenActions.contains(cn) )
            continue;
        QAction *act = new QAction(this);
        _m_reopenActions.insert(cn, act);
        acts << act;
        _m_mapperReopen->setMapping(act, cn);
        connect( act, SIGNAL( triggered() ), _m_mapperReopen, SLOT( map() ) );
    }
    return acts;
}

QMap<QString, QString> BTextEditor::_m_encodingsMap() const
{
    QMap<QString, QString> encodings;
    QStringList sl = _m_reopenActions.keys();
    for (int i = 0; i < sl.size(); ++i)
    {
        const QString &cn = sl.at(i);
        encodings.insert( cn, _m_reopenActions.value(cn)->text() );
    }
    return encodings;
}

//tools:additional

void BTextEditor::_m_resetSwitchDocumentMainAction()
{
    QAction *act = editorAction(SwitchDocumentMain);
    act->setEnabled(_m_currentDocument);
    if ( _m_currentDocument && _m_currentDocument->property("main").toBool() )
    {
        act->setIcon( QIcon( BCore::beqtIcon("list_add") ) );
        act->setText( tr("Unset main", "act text") );
        act->setToolTip( tr("Unset main document", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon( BCore::beqtIcon( hasMainDocument() ? QString("has_main_document") :
                                                                  QString("list_remove") ) ) );
        act->setText( tr("Set main", "act text") );
        act->setToolTip( tr("Set current document as main", "act toolTip") );
    }
}

void BTextEditor::_m_resetRecordMacroAction()
{
    QAction *act = editorAction(RecordMacroAction);
    if ( act->property("performing").toBool() )
    {
        act->setIcon( QIcon( BCore::beqtIcon("player_stop") ) );
        act->setText( tr("Stop recording", "act text") );
        act->setToolTip( tr("Stop recording macro", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon( BCore::beqtIcon("player_record") ) );
        act->setText( tr("Start recording", "act text") );
        act->setToolTip( tr("Start recording macro", "act toolTip") );
    }
}

void BTextEditor::_m_resetShowHideMacrosAction()
{
    QAction *act = editorAction(ShowHideMacrosConsole);
    if ( _m_recorderConsole->isVisible() )
    {
        act->setIcon( QIcon( BCore::beqtIcon("console_invisible") ) );
        act->setText( tr("Hide", "act text") );
        act->setToolTip( tr("Hide macros console", "act toolTip") );
    }
    else
    {
        act->setIcon( QIcon( BCore::beqtIcon("console_visible") ) );
        act->setText( tr("Show", "act text") );
        act->setToolTip( tr("Show macros console", "act toolTip") );
    }
}

void BTextEditor::_m_autoselectDocumentSyntax(BTextEditorDocument *document)
{
    if (!document)
        return;
    QString fn = document->fileName();
    BAbstractFileType *ft = _m_defaultFileType;
    for (int i = 0; i < _m_userFileTypes.size(); ++i)
    {
        if ( _m_userFileTypes.at(i)->matchesFileName(fn) )
        {
            ft = _m_userFileTypes.at(i);
            break;
        }
    }
    document->setSyntax( ft->syntax() );
}

void BTextEditor::_m_setCmboxSyntax(const QString &syntaxType)
{
    _m_cmboxSyntax->blockSignals(true);
    _m_cmboxSyntax->setCurrentIndex( _m_cmboxSyntax->findText(syntaxType) );
    _m_cmboxSyntax->blockSignals(false);
}

void BTextEditor::_m_textReplaced(int count)
{
    emit showMessage(tr("Replaced:", "stbar message") + " " + QString::number(count), _m_MessageTimeout);
}

//clipboard

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
    _m_retranslateAction( ReopenFileAction, tr("Reopen", "act text"),
                          tr("Reopen current document using encoding", "act toolTip"),
                          tr("Use this action to reopen current document using another encoding", "act WhatsThis") );
    _m_retranslateAction( RecentFilesAction, tr("Recent files", "act text"),
                          tr("Open recently opened files", "act toolTip"),
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
    _m_resetRecordMacroAction(); //TODO: whatsThis
    _m_retranslateAction( ClearMacroAction, tr("Clear macro", "act text"),
                          tr("Clear last recorded or loaded macro", "act toolTip") );
    _m_retranslateAction( PlayMacroAction, tr("Play", "act text"), tr("Play macro", "act toolTip") ); //TODO
    _m_resetShowHideMacrosAction(); //TODO: whatsThis
    _m_retranslateAction( LoadMacroAction, tr("Load...", "act text"), tr("Load macro...", "act toolTip") ); //TODO
    _m_retranslateAction( SaveMacroAction, tr("Save as...", "act text"), tr("Save macro as...", "act toolTip") );
    _m_retranslateAction( TextMacrosAction, tr("Insert text macro", "act text"),
                          tr("Insert text macro into current document", "act toolTip") );
    //menus
    _m_retranslateMenu( FileMenu, tr("File", "mnu title") );
    _m_retranslateMenu( EditMenu, tr("Edit", "mnu title") );
    _m_retranslateMenu( DocumentMenu, tr("Document", "mnu title") );
    _m_retranslateMenu( MacrosMenu, tr("Macros", "mnu title") );
    _m_retranslateReopenMenu();
    //toolBars
    _m_toolBars.value(FileMenu)->setWindowTitle( tr("File", "tbar windowTitle") );
    _m_toolBars.value(EditMenu)->setWindowTitle( tr("Edit", "tbar windowTitle") );
    _m_toolBars.value(DocumentMenu)->setWindowTitle( tr("Document", "tbar windowTitle") );
    _m_toolBars.value(MacrosMenu)->setWindowTitle( tr("Macros", "tbar windowTitle") );
}

void BTextEditor::_m_clipboardDataChanged()
{
    bool b = !QApplication::clipboard()->text().isEmpty();
    for (int i = 0; i < _m_twgt->count(); ++i)
        _m_document(i)->setClipboardHasText(b);
}

//document

void BTextEditor::_m_documentSwitchRequested()
{
    int count = _m_twgt->count();
    if (!_m_currentDocument || count < 2)
        return;
    int ind = _m_twgt->currentIndex();
    _m_twgt->setCurrentIndex( (ind < count - 1) ? ind + 1 : 0 );
}

void BTextEditor::_m_documentFileNameChanged(const QString &fileName)
{
    int ind = _m_twgt->indexOf( qobject_cast<BTextEditorDocument *>( sender() )->editWidget() );
    _m_twgt->setTabText( ind, QFileInfo(fileName).fileName() );
    _m_twgt->setTabToolTip(ind, fileName);
}

void BTextEditor::_m_documentModificationChanged(bool modified)
{
    BTextEditorDocument *ted = qobject_cast<BTextEditorDocument *>( sender() );
    int ind = _m_twgt->indexOf( ted->editWidget() );
    _m_twgt->setTabText( ind, (modified ? "*" : "") + QFileInfo( ted->fileName() ).fileName() );
    editorAction(SaveDocumentAction)->setEnabled(modified);
}

void BTextEditor::_m_documentSelectionChanged(bool hasSelection)
{
    editorAction(SwitchSelectedTextLayoutAction)->setEnabled(
                _m_currentDocument && !_m_currentDocument->isReadOnly() && hasSelection &&
                _m_keyboardLayoutMaps.contains(_m_currentKeyboardLayoutMap) );
    _m_findDlg->setSelectionAvailable(hasSelection);
}

void BTextEditor::_m_documentMaxLineLengthReached()
{
    emit showMessage(tr("Maximum line length reached!", "stbar message"), _m_MessageTimeout);
}

//find dialog

void BTextEditor::_m_findNext()
{
    if ( _m_currentDocument.isNull() )
        return;
    QString text = _m_findDlg->text();
    if ( !_m_currentDocument->find( text, _m_findDlg->findFlags(), _m_findDlg->cyclic() ) )
        emit showMessage(tr("Could not find", "stbar message") + " \"" + text + "\"", _m_MessageTimeout);
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
    if ( _m_currentDocument.isNull() )
        return;
    QString text = _m_findDlg->text();
    QString newText = _m_findDlg->newText();
    Qt::CaseSensitivity cs = _m_findDlg->caseSensitivity();
    int count = 0;
    bool b = true;
    for (int i = 0; i < _m_twgt->count(); ++i)
    {
        BTextEditorDocument *ted = _m_document(i);
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

//other

void BTextEditor::_m_reopen(const QString &codecName)
{
    if ( _m_currentDocument.isNull() || !BTextEditorDocument::checkEncoding(codecName) ||
         ( _m_currentDocument->isModified() && !_m_reloadModifiedQuestion( _m_currentDocument->fileName() ) ) )
        return;
    if ( _m_currentDocument->reopen(codecName) )
        _m_updateEncoding(codecName);
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
        _m_lblColumnValue->setText("----");
}

void BTextEditor::_m_updateEncoding(const QString &codecName)
{
    if ( codecName.isEmpty() )
        _m_lblEncodingValue->setText(_m_defaultEncoding);
    else
        _m_lblEncodingValue->setText(codecName);
}

void BTextEditor::_m_twgtCurrentChanged(int index)
{
    _m_currentDocument = _m_document(index);
    bool da = !_m_currentDocument.isNull();
    _m_findDlg->setDocumentAvailable(da);
    if (da)
        _m_findDlg->setReplaceAvailable( !_m_currentDocument->isReadOnly() );
    QString stext = da ? _m_currentDocument->selectedText() : "";
    _m_documentSelectionChanged( !stext.isEmpty() );
    bool b = da && QFileInfo( _m_currentDocument->fileName() ).exists();
    editorAction(ReopenFileAction)->setEnabled(b);
    editorAction(SaveDocumentAction)->setEnabled( da && _m_currentDocument->isModified() );
    editorAction(SaveDocumentAsAction)->setEnabled(da);
    editorAction(SaveAllDocumentsAction)->setEnabled(da);
    editorAction(CloseDocumentAction)->setEnabled(da);
    editorAction(CloseAllDocumentsAction)->setEnabled(da);
    editorAction(UndoAction)->setEnabled( da && _m_currentDocument->isUndoAvailable() );
    editorAction(RedoAction)->setEnabled( da && _m_currentDocument->isRedoAvailable() );
    editorAction(CutAction)->setEnabled( da && _m_currentDocument->isCutAvailable() );
    editorAction(CopyAction)->setEnabled( da && _m_currentDocument->isCopyAvailable() );
    editorAction(PasteAction)->setEnabled( da && _m_currentDocument->isPasteAvailable() );
    _m_resetSwitchDocumentMainAction();
    editorAction(MakeBookmarkAction)->setEnabled(da);
    editorAction(GotoNextBookmarkAction)->setEnabled(da && _m_currentDocument->hasBookmarks() );
    editorAction(SwitchBlockModeAction)->setEnabled(da);
    editorAction(SwitchSelectedTextLayoutAction)->setEnabled( da && _m_currentDocument->isCopyAvailable() );
    editorAction(FindAction)->setEnabled(da);
    editorAction(TextMacrosAction)->setEnabled( da && !editorAction(TextMacrosAction)->menu()->isEmpty() );
    emit currentDocumentChanged(da ? _m_currentDocument->fileName() : "");
    emit documentAvailableChanged(da);
    if (da)
        _m_setCmboxSyntax( _m_currentDocument->syntax().type() );
    _m_updateEncoding(da ? _m_currentDocument->codecName() : _m_defaultEncoding);
    if (!da)
    {
        _m_updateCursorPosition(-1, -1);
        return;
    }
    BTextEditorDocument::CursorPosition cp = _m_currentDocument->cursorPosition();
    _m_updateCursorPosition(cp.row, cp.column);
    _m_currentDocument->setFocusToEdit();
}

void BTextEditor::_m_twgtTabCloseRequested(int index)
{
    BTextEditorDocument *ted = _m_document(index);
    if (!ted)
        return;
    _m_closeDocument(ted);
}

void BTextEditor::_m_cmboxSyntaxCurrentIndexChanged(int index)
{
    if ( _m_currentDocument.isNull() || index < 0 || index > _m_userFileTypes.size() )
        return;
    _m_currentDocument->setSyntax( index ? _m_userFileTypes.at(index - 1)->syntax() : _m_defaultFileType->syntax() );
    _m_currentDocument->setFocusToEdit();
}
