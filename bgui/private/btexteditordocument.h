#ifndef BTEXTEDITORDOCUMENT_H
#define BTEXTEDITORDOCUMENT_H

class BSyntaxHighlighter;
class BMacroRecorder;

class QWidget;
class QPlainTextEdit;
class QMenu;
class QAction;
class QEvent;
class QKeyEvent;
class QMouseEvent;

#include "../bkeyboardlayoutmap.h"
#include "../bsyntax.h"

#include <QObject>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <QList>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QRegExp>
#include <QChar>
#include <QPoint>
#include <QMap>
#include <QPointer>

class BTextEditorDocument : public QObject
{
    Q_OBJECT
public:
    struct CursorPosition
    {
        int row;
        int column;
    };
    //
    static const QStringList EncodingsValid;
    static const QString EncodingDef;
    static const QString FontFamilyDef;
    static const int FontPointSizeMin;
    static const int FontPointSizeDef;
    static const int FontPointSizeMax;
    static const int LineLengthMin;
    static const int LineLengthDef;
    static const int LineLengthMax;
    static const int TabWidthDef;
    static const QList<int> TabWidthsValid;
    static const bool BlockModeDef;
    //
    static bool checkEncoding(const QString &codecName);
    static bool checkFontFamily(const QString &family);
    static bool checkFontPointSize(int pointSize);
    static bool checkLineLength(int length);
    static bool checkTabWidth(int width);
    //
    explicit BTextEditorDocument(const QString &fileName, const QString &codecName, QObject *parent = 0);
    ~BTextEditorDocument();
    //
    bool eventFilter(QObject *object, QEvent *event);
    void setMacroRecorder(BMacroRecorder *recorder);
    void setReadOnly(bool readOnly);
    void setFontFamily(const QString &family);
    void setFontPointSize(int pointSize);
    void setBlockMode(bool enabled);
    void setSyntax(const BSyntax &syntax);
    void setLineLength(int length);
    void setTabWidth(int width);
    void setClipboardHasText(bool b);
    const QString &fileName() const;
    const QString &codecName() const;
    bool isReadOnly() const;
    bool isCutAvailable() const;
    bool isCopyAvailable() const;
    bool isPasteAvailable() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    bool isModified() const;
    bool hasBookmarks() const;
    bool hasSelection() const;
    const BSyntax &syntax() const;
    CursorPosition cursorPosition() const;
    QPlainTextEdit *editWidget() const;
    QString text() const;
    QString selectedText() const;
    bool open( const QString &fileName, const QString &codecName = QString() );
    bool reopen( const QString &codecName = QString() );
    bool save( const QString &codecName = QString() );
    bool saveAs( const QString &fileName, const QString &codecName = QString() );
    bool find(const QString &text, QTextDocument::FindFlags flags, bool cyclic);
    bool replace(const QString &newText);
    int replaceInSelection(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInDocument(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    void insertText(const QString &text);
    void setText(const QString &text);
    void deselect();
    void makeBookmark();
    void gotoNextBookmark();
    void switchSelectedTextLayout(const BKeyboardLayoutMap &klm);
public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void setFocusToEdit();
private:
    struct _m_Bookmark
    {
        QTextBlock block;
        int posInBlock;
        bool operator==(const _m_Bookmark &other) const
        {
            return block == other.block && posInBlock == other.posInBlock;
        }
    };
    //
    QString _m_fileName;
    int _m_lineLength;
    int _m_tabWidth;
    QString _m_codecName;
    QPointer<QPlainTextEdit> _m_edit;
    BSyntaxHighlighter *_m_highlighter;
    BSyntax _m_syntax;
    BMacroRecorder *_m_recorder;
    QMenu *_m_mnuContext;
      QAction *_m_actUndo;
      QAction *_m_actRedo;
      //separator
      QAction *_m_actCut;
      QAction *_m_actCopy;
      QAction *_m_actPaste;
      QAction *_m_actDelete;
      //separator
      QAction *_m_actSelectAll;
    QTextCharFormat _m_tcfHighlighted;
    bool _m_blockMode;
    QList<int> _m_highlightPosList;
    QList<_m_Bookmark> _m_bookmarks;
    //
    void _m_initWidget();
    void _m_initMenu();
    void _m_initAction(QAction *&action, const QString &iconName,
                       const QString &shortcut = QString(), bool enabled = false);
    bool _m_setFileName(const QString &fileName);
    void _m_processSavedText(QString &text);
    void _m_addSpaces(QString &text) const;
    int _m_replaceInSelectionLines(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    int _m_replaceInSelectionBlocks(const QString &text, const QString &newText, Qt::CaseSensitivity cs);
    bool _m_handleKeyPressEvent(QObject *object, QKeyEvent *event);
    bool _m_handleMousePressEvent(QObject *object, QMouseEvent *event);
    bool _m_handleMouseDoubleClickEvent(QObject *object, QMouseEvent *event);
    void _m_handleReturn();
    void _m_handleEnd(bool ctrl = false);
    void _m_handleShiftEnd();
    void _m_handleBackspace();
    void _m_handleDelete();
    void _m_handleCtrlBackspace();
    void _m_handleCtrlDelete();
    void _m_handleCtrlLeft();
    void _m_handleCtrlRight();
    void _m_handleLeft(bool shift = false);
    void _m_handleSymbol(const QChar &c);
    void _m_move(int key);
    void _m_highlightBrackets();
    void _m_highlightBracket(int pos, bool highlight, bool error);
private slots:
    void _m_retranslateUi();
    void _m_cutAvailableChanged(bool available);
    void _m_undoAvailableChanged(bool available);
    void _m_redoAvailableChanged(bool available);
    void _m_modificationChanged(bool modified);
    void _m_documentBlockCountChanged(int count);
    void _m_editCursorPositionChanged();
    void _m_editSelectionChanged();
    void _m_editCustomContextMenuRequested(const QPoint &pos);
    void _m_deleteSelection();
    void _m_selectAll();
    void _m_futureWatcherFinished();
    void _m_gotoLine(int index, bool select);
signals:
    void requestSwitchDocument();
    void fileNameChanged(const QString &fileName);
    void cutAvailableChanged(bool available);
    void copyAvailableChanged(bool available);
    void undoAvailableChanged(bool available);
    void redoAvailableChanged(bool available);
    void modificationChanged(bool modified);
    void selectionChanged(bool hasSelection);
    void hasBookmarkChanged(bool b);
    void cursorPositionChanged(int row, int column);
    void maxLineLengthReached();
};

#endif // BTEXTEDITORDOCUMENT_H
