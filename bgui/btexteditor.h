#ifndef BTEXTEDITOR_H
#define BTEXTEDITOR_H

class BTextEditorDocument;
class BFindDialog;
class BOpenSaveDialog;
class BAbstractSettingsTab;
class BMacroRecorder;

class QObject;
class QAction;
class QSignalMapper;
class QEvent;
class QToolBar;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QMenu;
class QLabel;
class QPlainTextEdit;
class QTabWidget;

#include "bsyntax.h"
#include "babstractfiletype.h"
#include "bkeyboardlayoutmap.h"

#include <QWidget>
#include <QStringList>
#include <QString>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QVariantMap>
#include <QRect>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BTextEditor : public QWidget
{
    Q_OBJECT
public:
    enum Menu
    {
        FileMenu,
        EditMenu,
        DocumentMenu,
        MacrosMenu
    };
    enum Action
    {
        //FileMenu
          NewDocumentAction,
          OpenFileAction,
          ReopenFileAction,
          RecentFilesAction,
          //separator
          SaveDocumentAction,
          SaveDocumentAsAction,
          SaveAllDocumentsAction,
          //separator
          CloseDocumentAction,
          CloseAllDocumentsAction,
        //EditMenu
          UndoAction,
          RedoAction,
          //separator
          CutAction,
          CopyAction,
          PasteAction,
          //separator
          SwitchSelectedTextLayoutAction,
          //separator
          FindAction,
          FindNextAction,
        //DocumentMenu
          SwitchDocumentMain,
          //separator
          MakeBookmarkAction,
          GotoNextBookmarkAction,
          //separator
          SwitchBlockModeAction,
        //MacrosMenu
          RecordMacroAction,
          ClearMacroAction,
          //separator
          PlayMacroAction,
          ShowHideMacrosConsole,
          //separator
          LoadMacroAction,
          SaveMacroAction,
          //separator
          TextMacrosAction
    };
    //
    class SettingsOptions
    {
    public:
        bool toolBarIconSize;
        bool fontFamily;
        bool fontPointSize;
        bool defaultEncoding;
        bool lineLength;
        bool tabWidth;
        bool keyboardLayoutMap;
        bool macrosDir;
        //
        SettingsOptions();
    };
    //
    struct FindDialogSettings
    {
        QStringList textHistory;
        QStringList newTextHistory;
        bool caseSensitive;
        bool wholeWords;
        bool backwardOrder;
        bool cyclic;
    };
    //
    static const QString SettingsTabId;
    //
    static bool isFileOpenedGlobal( const QString &fileName, const QString &groupId = QString() );
    //
    explicit BTextEditor( QWidget *parent = 0, bool registerGlobally = false, const QString &groupId = QString() );
    ~BTextEditor();
    //
    bool eventFilter(QObject *object, QEvent *event);
    //settings:set
    void setUserFileTypes(QList<BAbstractFileType *> list);
    void setDefaultFileName(const QString &fileName);
    void setDefaultMacrosDir(const QString &dir);
    void setToolBarIconSize(int size);
    void setMacrosDir(const QString &dir);
    void setDefaultEncoding(const QString &codecName);
    void setFontFamily(const QString &family);
    void setFontPointSize(int pointSize);
    void setLineLength(int length);
    void setTabWidth(int width);
    void setKeyboardLayoutMap(const QString &description);
    void setBlockMode(bool enabled);
    void setFindDialogSettings(const FindDialogSettings &settings);
    void applySettings(const QVariantMap &settings);
    //settings:get
    bool isRegisteredGlobally() const;
    const QString &groupId() const;
    QList<BAbstractFileType *> userFileTypes() const;
    const QString &defaultFileName() const;
    const QString &macrosDir() const;
    int toolBarIconSize() const;
    const QString &defaultEncoding() const;
    const QString &fontFamily() const;
    int fontPointSize() const;
    int lineLength() const;
    int tabWidth() const;
    const QString &keyboardLayoutMap() const;
    bool blockMode() const;
    FindDialogSettings findDialogSettings() const;
    BAbstractSettingsTab *createSettingsTab(const SettingsOptions &opt) const;
    //settings:load/save
    void loadSettings( const QString &settingsGroup = QString() );
    void saveSettings( const QString &settingsGroup = QString() );
    //loadable content
    void loadTextMacros(const QString &dir);
    void loadTextMacros(const QStringList &dirs);
    void loadKeyboardLayoutMaps(const QString &dir);
    void loadKeyboardLayoutMaps(const QStringList &dirs);
    const QStringList &textMacrosDirs() const;
    const QStringList &keyboardLayoutMapsDirs() const;
    //gui:set
    void setTabsClosable(bool closable);
    void setTabsMovable(bool movable);
    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);
    //gui:get
    QToolBar *editorToolBar(Menu id) const;
    QList<QToolBar *> editorToolBars() const;
    QMenu *editorMenu(Menu id) const;
    QList<QMenu *> editorMenus() const;
    QAction *editorAction(Action id) const;
    QList<QAction *> editorActions(Menu id, bool includeSeparators = true) const;
    QList<QAction *> editorActions() const;
    QWidget *indicatorWidget() const;
    QWidget *widget(int index) const;
    //document
    bool isDocumentAvailable() const;
    bool hasMainDocument() const;
    bool isCurrentDocumentMain() const;
    QString currentDocumentFileName() const;
    QString currentDocumentText() const;
    QString currentDocumentSelectedText() const;
    QString mainDocumentFileName() const;
    QString mainDocumentText() const;
    QString mainDocumentSelectedText() const;
    //other
    bool askOnClose();
    bool isFileOpened(const QString &fileName) const;
    QStringList openedFiles( const QStringList &fileNames = QStringList() ) const;
public slots:
    bool performAction(int id);
    void newDocument( const QString &text = QString() );
    void openFile(const QString &fileName);
    void openFiles(const QStringList &fileNames);
    void insertText(const QString &text);
    void setText(const QString &text);
    void deselect();
private:
    enum _m_OpenMultipleQuestionResult
    {
        _m_CancelOpening = 0,
        _m_Open,
        _m_OpenReadonly
    };
    enum _m_CloseModifiedQuestionResult
    {
        _m_CancelClosing = 0,
        _m_CloseDiscard,
        _m_CloseSave
    };
    //
    static const QString _m_SyntaxTypeDef;
    static const QString _m_MacrosDirDef;
    //
    static const QRect _m_OpenSaveDlgGeometryDef;
    static const QString _m_OpenSaveDlgDirDef;
    //
    static const QRect _m_SelectFilesDlgGeometryDef;
    //
    static const int _m_MessageTimeout;
    static const int _m_RecentFilesMax;
    //
    static const QString _m_GroupTextEditor;
      static const QString _m_KeyMacrosDir;
      static const QString _m_KeyToolBarIconSize;
      static const QString _m_KeyDefaultEncoding;
      static const QString _m_KeyFontFamily;
      static const QString _m_KeyFontPointSize;
      static const QString _m_KeyLineLength;
      static const QString _m_KeyTabWidth;
      static const QString _m_KeyKeyboardLayoutMap;
      static const QString _m_KeyBlockMode;
      static const QString _m_KeyRecentFiles;
      static const QString _m_GroupFindDialog;
        static const QString _m_KeyTextHistory;
        static const QString _m_KeyNewTextHistory;
        static const QString _m_GroupOptions;
          static const QString _m_KeyCaseSensitive;
          static const QString _m_KeyWholeWords;
          static const QString _m_KeyBackwardOrder;
          static const QString _m_KeyCyclic;
      static const QString _m_GroupOpenSaveDialog;
        static const QString _m_KeyGeometry;
        static const QString _m_KeyDir;
      static const QString _m_GroupSelectFilesDialog;
        //_m_KeyDir
    //
    static QList<BTextEditor *> _m_instanceList;
    //
    static QList<BTextEditor *> _m_instances(const QString &groupId);
    static BTextEditorDocument *_m_document(QWidget *widget);
    static bool _m_checkReadOnly(const QString &fileName);
    static QAction *_m_createSeparator();
    //
    const bool _m_CRegistered;
    const QString _m_CGroupId;
    //
    //gui:visible
    QVBoxLayout *_m_vlt;
      QTabWidget *_m_twgt;
    //gui:invisible:indicator
    QPointer<QWidget> _m_wgtIndicator;
      QComboBox *_m_cmboxSyntax;
      //separator
      QLabel *_m_lblRowLabel;
      QLabel *_m_lblRowValue;
      QLabel *_m_lblColumnLabel;
      QLabel *_m_lblColumnValue;
      //separator
      QLabel *_m_lblEncodingLabel;
      QLabel *_m_lblEncodingValue;
    //gui:invisible:toolbars, menus, actions
    QMap<int, QToolBar *> _m_toolBars;
    QMap<int, QMenu *> _m_menus;
    QMap<int, QAction *> _m_actions;
    QMenu *_m_mnuReopenUnicode;
    QMenu *_m_mnuReopenEasternEurope;
    QMenu *_m_mnuReopenWesternEurope;
    QMenu *_m_mnuReopenEastAsia;
    QMenu *_m_mnuReopenSouthEastSouthWestAsia;
    QMenu *_m_mnuReopenMiddleEast;
    QMap<QString, QAction *> _m_reopenActions;
    //gui:other
    QPlainTextEdit *_m_recorderConsole;
    BFindDialog *_m_findDlg;
    //settings
    QList<BAbstractFileType *> _m_userFileTypes;
    QString _m_defaultFileName;
    QString _m_defaultMacrosDir;
    QString _m_macrosDir;
    int _m_toolBarIconSize;
    QString _m_defaultEncoding;
    QString _m_fontFamily;
    int _m_fontPointSize;
    int _m_lineLength;
    int _m_tabWidth;
    QString _m_currentKeyboardLayoutMap;
    bool _m_blockMode;
    //settings:other
    QRect _m_openSaveDlgGeometry;
    QString _m_openSaveDlgDir;
    QRect _m_selectFilesDlgGeometry;
    //main
    QPointer<BTextEditorDocument> _m_currentDocument;
    QSignalMapper *_m_mapperActions;
    QSignalMapper *_m_mapperReopen;
    QSignalMapper *_m_mapperRecent;
    QSignalMapper *_m_mapperTextMacros;
    BMacroRecorder *_m_recorder;
    BAbstractFileType *_m_defaultFileType;
    QMap<QString, BKeyboardLayoutMap> _m_keyboardLayoutMaps;
    //additional
    QStringList _m_textMacrosDirs;
    QStringList _m_keyboardLayoutMapsDirs;
    QStringList _m_openSaveDlgHistory;
    //
    //init:main
    void _m_init();
    void _m_initSettings();
    void _m_initOtherMembers();
    void _m_initGui();
    void _m_initIndicator();
    void _m_initToolBars();
    void _m_initMenus();
    void _m_initReopenMenu();
    void _m_initRecentFilesMenu();
    void _m_initRecorderConsole();
    void _m_initFindDialog();
    //retranslate
    void _m_retranslateReopenMenu();
    void _m_retranslateAction( Action id, const QString &text, const QString &toolTip = QString(),
                               const QString &whatsThis = QString() );
    void _m_retranslateMenu(Menu id, const QString &title);
    void _m_setReopenActionsText(const QStringList &codecNames, const QString &description);
    void _m_retranslateSwitchBlockModeAction();
    //actions:file
    void _m_newDocument( const QString &text = QString() );
    bool _m_openFile( const QString &fileName = QString() );
    bool _m_saveDocument(BTextEditorDocument *document = 0);
    bool _m_saveDocumentAs(BTextEditorDocument *document = 0);
    bool _m_saveAllDocuments();
    bool _m_closeDocument(BTextEditorDocument *document = 0);
    bool _m_closeAllDocuments();
    //actions:edit
    void _m_undo();
    void _m_redo();
    void _m_cut();
    void _m_copy();
    void _m_paste();
    void _m_switchSelectedTextLayout();
    void _m_find();
    //find, find next, replace next
    //actions:document
    void _m_switchDocumentMain();
    void _m_makeBookmark();
    void _m_gotoNextBookmark();
    void _m_switchBlockMode();
    //actions:macros
    void _m_recordMacro();
    void _m_playMacro();
    void _m_showHideMacrosConsole();
    bool _m_loadMacro();
    bool _m_saveMacro();
    //tools:main
    BTextEditorDocument *_m_addDocument(const QString &fileName);
    BTextEditorDocument *_m_document(int index) const;
    BTextEditorDocument *_m_mainDocument() const;
    void _m_loadRecentFiles(const QStringList &fileNames);
    QStringList _m_saveRecentFiles() const;
    void _m_addRecentFile( const QString &fileName, const QString &oldFileName = QString() );
    BOpenSaveDialog *_m_createOpenSaveDialog(bool openMode, BTextEditorDocument *document = 0);
    void _m_handleOpenSaveDialog(BOpenSaveDialog *dialog);
    //tools:messages
    void _m_alreadyOpenedInformation(const QString &fileName);
    bool _m_reloadModifiedQuestion(const QString &fileName);
    _m_CloseModifiedQuestionResult _m_closeModifiedQuestion(const QString &fileName);
    _m_OpenMultipleQuestionResult _m_openMultipleQuestion(const QString &fileName);
    void _m_saveFailureError(const QString &fileName);
    //tools:create
    QToolBar *_m_createToolBar(Menu id, const QString &objectName);
    QMenu *_m_createMenu(Menu id, const QString &objectName);
    QAction *_m_createAction(Action id, QToolBar *tbar, const QString &iconName,
                             const QString &shortcut = QString(), bool enabled = false);
    QAction *_m_createMenuAction(Action id, QToolBar *tbar, const QString &iconName, bool enabled = false);
    QList<QAction *> _m_createReopenActions(const QStringList &codecNames);
    QMap<QString, QString> _m_encodingsMap() const;
    //tools:additional
    void _m_resetSwitchDocumentMainAction();
    void _m_resetRecordMacroAction();
    void _m_resetShowHideMacrosAction();
    void _m_autoselectDocumentSyntax(BTextEditorDocument *document);
    void _m_setCmboxSyntax(const QString &syntaxType);
    void _m_textReplaced(int count);
private slots:
    //global
    void _m_retranslateUi();
    void _m_clipboardDataChanged();
    //document
    void _m_documentSwitchRequested();
    void _m_documentFileNameChanged(const QString &fileName);
    void _m_documentModificationChanged(bool modified);
    void _m_documentSelectionChanged(bool hasSelection);
    void _m_documentMaxLineLengthReached();
    //find dialog
    void _m_findNext();
    void _m_replaceNext();
    void _m_replaceInSelection();
    void _m_replaceInDocument();
    void _m_replaceInAllDocuments();
    //other
    void _m_reopen(const QString &codecName);
    void _m_updateCursorPosition(int row, int column);
    void _m_updateEncoding(const QString &codecName);
    void _m_twgtCurrentChanged(int index);
    void _m_twgtTabCloseRequested(int index);
    void _m_cmboxSyntaxCurrentIndexChanged(int index);
signals:
    void currentDocumentChanged(const QString &fileName);
    void documentAvailableChanged(bool available);
    void showMessage(const QString &message, int timeout);
};

#endif // BTEXTEDITOR_H
