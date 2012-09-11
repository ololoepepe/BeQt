#ifndef BTEXTEDITOR_H
#define BTEXTEDITOR_H

class BTextEditorDocument;
class BEncoding;
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
class QStackedLayout;
class QLabel;
class QPlainTextEdit;

#include "bsyntax.h"

#include <QWidget>
#include <QStringList>
#include <QString>
#include <QList>
#include <QMap>
#include <QUrl>
#include <QPointer>
#include <QChar>
#include <QVariantMap>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BTextEditor : public QWidget
{
    Q_OBJECT
public:
    enum ToolBar
    {
        SwitchDocumentToolBar,
        NewOpenToolBar,
        SaveToolBar,
        CloseToolBar,
        UndoRedoToolBar,
        ClipboardToolBar,
        DocumentToolBar,
        SearchToolBar,
        MacrosToolBar
    };
    enum Action
    {
        NewDocumentAction,
        OpenFileAction,
        ReopenFileAction,
        RecentFilesAction,
        SaveDocumentAction,
        SaveDocumentAsAction,
        SaveAllDocumentsAction,
        CloseDocumentAction,
        CloseAllDocumentsAction,
        UndoAction,
        RedoAction,
        CutAction,
        CopyAction,
        PasteAction,
        SwitchDocumentMain,
        MakeBookmarkAction,
        GotoNextBookmarkAction,
        SwitchBlockModeAction,
        SwitchSelectedTextLayoutAction,
        FindAction,
        FindNextAction,
        ReplaceNextAction,
        RecordMacroAction,
        PlayMacroAction,
        ShowHideMacros,
        LoadMacroAction,
        SaveMacroAction,
        OpenMacrosDirAction
    };
    enum Menu
    {
        FileMenu,
        EditMenu,
        DocumentMenu,
        MacrosMenu
    };
    //
    struct FileTypeInfo
    {
        QString fileName;
        QStringList suffixes;
        QString description;
    };
    //
    typedef QList<FileTypeInfo> FileTypeInfoList;
    //
    static bool isFileOpenedGlobal(const QString &fileName);
    static QStringList openedFilesGlobal( const QStringList &fileNames = QStringList() );
    static void applySettingsGlobal(const QVariantMap &settings);
    //
    explicit BTextEditor( QWidget *parent = 0, const QString &id = QString() );
    ~BTextEditor();
    //
    bool eventFilter(QObject *object, QEvent *event);
    void setAcceptableFileTypes(const FileTypeInfoList &list);
    void setMacrosDefaultDir(const QString &dir);
    void setDefaultEncoding(const QString &codecName);
    void setFontFamily(const QString &family);
    void setFontPointSize(int pointSize);
    void setLineLength(int length);
    void setTabWidth(int width);
    void setMessageTimeout(int timeout);
    void setSyntaxes(const QList<BSyntax> &list);
    void loadSyntaxes(const QString &dir);
    void loadTextMacros(const QString &dir, bool clear = false);
    void loadKeyboardLayoutsMap( const QString &fileName, const QString &codecName = QString() );
    void applySettings(const QVariantMap &settings);
    void addUpperWidget(QWidget *widget);
    void addLowerWidget(QWidget *widget);
    bool askOnApplicationQuit();
    const QString &id() const;
    const FileTypeInfoList &acceptableFileTypes() const;
    const QString &macrosDefaultDir() const;
    const QString &defaultEncoding() const;
    const QString &fontFamily() const;
    int fontPointSize() const;
    int lineLength() const;
    int tabWidth() const;
    int messageTimeout() const;
    const QString &keyboardLayoutsMap() const;
    const QList<BSyntax> &syntaxes() const;
    bool isFileOpened(const QString &fileName) const;
    QStringList openedFiles( const QStringList &fileNames = QStringList() ) const;
    BAbstractSettingsTab *settingsTab() const;
    QToolBar *toolBar(ToolBar tbarId) const;
    QList<QToolBar *> toolBars() const;
    QAction *editorAction(Action actId) const;
    QList<QAction *> editorActions(ToolBar tbarId) const;
    QWidget *widgetForAction(Action actId) const;
    QWidget *indicatorWidget() const;
    QMenu *editorMenu(Menu mnuId) const;
    bool isDocumentAvailable() const;
    QString currentDocumentFileName() const;
    QString currentDocumentText() const;
    QString currentDocumentSelectedText() const;
    bool isCurrentDocumentMain() const;
    bool hasMainDocument() const;
    QString mainDocumentFileName() const;
    QString mainDocumentText() const;
    QString mainDocumentSelectedText() const;
public slots:
    bool performAction(int actId);
    void reopen(const QString &codecName);
    void newDocument(const QString &text);
    void openFile(const QString &fileName);
    void openFiles(const QStringList &fileNames);
    void insertText(const QString &text);
    void setText(const QString &text);
    void deselect();
    void saveSettings();
protected:
    void changeEvent(QEvent *event);
    virtual void retranslateUi();
private:
    typedef QList<BTextEditorDocument *> _m_TextEditorDocumentList;
    typedef QMap<int, QToolBar *> _m_ToolBarMap;
    typedef QMap<int, QAction *> _m_ActionMap;
    //
    enum _m_LockOperation
    {
        _m_Lock,
        _m_Unlock
    };
    //
    const QString _m_CId;
    //
    int _m_lineLength;
    int _m_tabWidth;
    int _m_messageTimeout;
    FileTypeInfoList _m_fileTypes;
    QString _m_codecName;
    QMap<QString, BEncoding *> _m_encodings;
    QString _m_fontFamily;
    int _m_fontPointSize;
    QString _m_dir;
    QStringList _m_history;
    QString _m_macrosDir;
    QString _m_macrosDefDir;
    _m_TextEditorDocumentList _m_documents;
    BFindDialog *_m_findDlg;
    BMacroRecorder *_m_recorder;
    QPlainTextEdit *_m_recorderConsole;
    QComboBox *_m_cmboxTextMacros;
    bool _m_blockMode;
    QList<BSyntax> _m_syntaxes;
    QString _m_klmFileName;
    QMap<QChar, QChar> _m_directMap;
    QMap<QChar, QChar> _m_reverseMap;
    QList<QChar> _m_directUnique;
    QList<QChar> _m_reverseUnique;
    BTextEditorDocument *_m_currentDocument;
    _m_ToolBarMap _m_toolBarMap;
    _m_ActionMap _m_actionMap;
    QMap<int, QMenu *> _m_menuMap;
    QSignalMapper *_m_mapperActions;
    QSignalMapper *_m_mapperReopen;
    QSignalMapper *_m_mapperRecent;
    QPointer<QWidget> _m_wgtIndicator;
      //syntax
      QComboBox *_m_cmboxSyntax;
      //cursor position
      QLabel *_m_lblRowLabel;
      QLabel *_m_lblRowValue;
      QLabel *_m_lblColumnLabel;
      QLabel *_m_lblColumnValue;
      //encoding
      QLabel *_m_lblEncodingLabel;
      QLabel *_m_lblEncodingValue;
    //
    QComboBox *_m_cmbox;
    //
    QVBoxLayout *_m_vlt;
      QStackedLayout *_m_slt;
    //
    void _m_loadSettings();
    void _m_saveSettings();
    void _m_initEncodings();
    void _m_initGui();
    void _m_initIndicator();
    void _m_initFindDialog();
    void _m_initToolBars();
    void _m_initMenus();
    void _m_initRecorderConsole();
    void _m_initReopenMenu(QToolBar *tbar = 0);
    void _m_initRecentFilesMenu(QToolBar *tbar = 0);
    QToolBar *_m_createToolBar(int id, const QString &objectName);
    QMenu *_m_createMenu(int id, const QString &objectName);
    QAction *_m_createAction(int id, const QString &iconFileName, const QString &shortcut = QString(),
                             QToolBar *tbar = 0, bool enabled = false);
    void _m_retranslateUi();
    void _m_retranslateReopenMenu();
    void _m_retranslateAction( Action actId, const QString &text, const QString &toolTip = QString(),
                               const QString &whatsThis = QString() );
    void _m_retranslateMenu(Menu mnuId, const QString &title);
    void _m_retranslateSwitchBlockModeAction();
    void _m_newDocument( const QString &text = QString() );
    void _m_openFile( const QString &fileName = QString() );
    bool _m_saveDocument(BTextEditorDocument *document = 0);
    bool _m_saveDocumentAs(BTextEditorDocument *document = 0);
    bool _m_saveAllDocuments();
    bool _m_closeDocument(BTextEditorDocument *document = 0);
    bool _m_closeAllDocuments();
    void _m_switchDocumentMain();
    void _m_switchBlockMode();
    void _m_recordMacro();
    void _m_playMacro();
    void _m_showHideMacros();
    bool _m_loadMacro();
    bool _m_saveMacro();
    void _m_resetSwitchDocumentMainAction();
    void _m_resetRecordMacroAction();
    void _m_resetShowHideMacrosAction();
    BTextEditorDocument *_m_addDocument(const QString &fileName);
    void _m_addRecentFile( const QString &fileName, const QString &oldFileName = QString() );
    bool _m_reopenQuestion(const QString &fileName);
    int _m_reopenReadonlyQuestion(const QString &fileName);
    void _m_alreadyOpenedWarning(const QString &fileName);
    int _m_closeQuestion(const QString &fileName);
    void _m_failedToSaveError(const QString &fileName);
    void _m_resetCmboxItem(BTextEditorDocument *document, bool nameChanged = false);
    FileTypeInfo _m_defFileType() const;
    BOpenSaveDialog *_m_createOpenSaveDialog(bool openMode, BTextEditorDocument *document = 0);
    void _m_handleOpenSaveDialog(BOpenSaveDialog *dialog);
    QString _m_fileDialogFilter() const;
    QStringList _m_filterAcceptableFiles(const QStringList &fileNames) const;
    void _m_handleUrls(const QList<QUrl> &urls);
    QString _m_cmboxToolTip() const;
    BTextEditorDocument *_m_mainDocument() const;
    void _m_textReplaced(int count);
    void _m_setDocumentSyntax(BTextEditorDocument *document);
    void _m_setCmboxSyntax(const BSyntax &syntax);
private slots:
    void _m_checkPasteAvailable();
    void _m_updateCursorPosition(int row, int column);
    void _m_updateEncoding(const QString &codecName);
    void _m_cmboxCurrentIndexChanged(int index);
    void _m_cmboxTextMacrosActivated(int index);
    void _m_cmboxSyntaxCurrentIndexChanged(int index);
    void _m_documentSwitchRequested();
    void _m_documentFileNameChanged(const QString &fileName);
    void _m_documentModificationChanged(bool modified);
    void _m_documentSelectionChanged(bool hasSelection);
    void _m_findNext();
    void _m_replaceNext();
    void _m_replaceInSelection();
    void _m_replaceInDocument();
    void _m_replaceInAllDocuments();
    void _m_maxLineLengthReached();
signals:
    void currentDocumentChanged(const QString &fileName);
    void documentAvailableChanged(bool available);
    void showMessage(const QString &message, int timeout);
};

#endif // BTEXTEDITOR_H
