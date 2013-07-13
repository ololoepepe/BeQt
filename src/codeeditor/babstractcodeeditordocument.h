#ifndef BABSTRACTCODEEDITORDOCUMENT_H
#define BABSTRACTCODEEDITORDOCUMENT_H

class BAbstractCodeEditorDocumentPrivate;
class BCodeEditor;
class BAbstractDocumentDriver;
class BCodeEditPrivate;
class BCodeEditorPrivate;

class QFont;
class QPoint;
class QColor;

#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QWidget>
#include <QList>
#include <QString>
#include <QTextDocument>

/*============================================================================
================================ BAbstractCodeEditorDocument =================
============================================================================*/

class BAbstractCodeEditorDocument : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BAbstractCodeEditorDocument)
public:
    explicit BAbstractCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BAbstractCodeEditorDocument();
protected:
    explicit BAbstractCodeEditorDocument(BAbstractCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    virtual void setReadOnly(bool ro) = 0;
    virtual void setModification(bool modified) = 0;
    virtual void setEditFont(const QFont &fnt) = 0;
    virtual void setEditTabWidth(BeQt::TabWidth tw) = 0;
    virtual void setLineNumberWidgetVisible(bool b) = 0;
    virtual void setCurrentLineHighlightingEnabled(bool b) = 0;
    virtual void setHighlightedLineColor(const QColor &c) = 0;
    virtual bool isReadOnly() const = 0;
    virtual bool isModified() const = 0;
    virtual bool hasSelection() const = 0;
    virtual bool isCutAvailable() const = 0;
    virtual bool isCopyAvailable() const = 0;
    virtual bool isPasteAvailable() const = 0;
    virtual bool isUndoAvailable() const = 0;
    virtual bool isRedoAvailable() const = 0;
    virtual QFont editFont() const = 0;
    virtual BeQt::TabWidth editTabWidth() const = 0;
    virtual bool lineNumberWidgetVisible() const = 0;
    virtual bool currentLineHighlightingEnabled() const = 0;
    virtual QColor highlightedLineColor() const = 0;
    virtual QPoint cursorPosition() const = 0;
    virtual QString text(bool full = false) const = 0;
    virtual QString selectedText(bool full = false) const = 0;
    virtual QPoint selectionStart() const = 0;
    virtual QPoint selectionEnd() const = 0;
    virtual bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true) = 0;
    virtual bool replaceNext(const QString &newText) = 0;
    virtual int replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs) = 0;
    virtual int replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs) = 0;
    void init();
    void setFileType(BAbstractFileType *ft);
    void setRecognizedBrackets(const BAbstractFileType::BracketPairList &list);
    void setBracketHighlightingEnabled(bool enabled);
    void setFileName(const QString &fn);
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    void setAsyncProcessingMinimumLength(int length);
    bool load(BAbstractDocumentDriver *driver, const QString &fileName = QString());
    bool load(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString());
    bool save(BAbstractDocumentDriver *driver, const QString &fileName = QString());
    bool save(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName = QString());
    bool waitForProcessed(int msecs = 30 * BeQt::Second);
    BAbstractFileType *fileType() const;
    QString fileTypeId() const;
    BAbstractFileType::BracketPairList recognizedBrackets() const;
    bool isBracketHighlightingEnabled() const;
    QString fileName() const;
    QTextCodec *codec() const;
    QString codecName() const;
    int asyncProcessingMinimumLength() const;
    bool isBuisy() const;
    BCodeEditor *editor() const;
public Q_SLOTS:
    void setFocus();
    void activateWindow();
    void setText(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void insertText(const QString &txt);
    void moveCursor(const QPoint &pos);
    void selectText(const QPoint &start, const QPoint &end);
    void selectText(int start, int end);
    void selectLines(int firstLine, int lastLine);
    void selectAll();
    void deselectText();
    void cut();
    void copy();
    void paste();
    void deleteSelection();
    void undo();
    void redo();
    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);
protected:
    virtual QWidget *createEdit(QTextDocument **doc = 0) = 0;
    virtual void setFocusImplementation() = 0;
    virtual void activateWindowImplementation() = 0;
    virtual void setTextImplementation(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte) = 0;
    virtual void insertTextImplementation(const QString &txt) = 0;
    virtual void moveCursorImplementation(const QPoint &pos) = 0;
    virtual void selectTextImplementation(const QPoint &start, const QPoint &end) = 0;
    virtual void selectTextImplementation(int start, int end) = 0;
    virtual void selectLinesImplementation(int firstLine, int lastLine) = 0;
    virtual void selectAllImplementation() = 0;
    virtual void deselectTextImplementation() = 0;
    virtual void cutImplementation() = 0;
    virtual void copyImplementation() = 0;
    virtual void pasteImplementation() = 0;
    virtual void deleteSelectionImplementation() = 0;
    virtual void undoImplementation() = 0;
    virtual void redoImplementation() = 0;
    virtual void highlightBrackets() = 0;
    virtual void installDropHandler(QObject *handler) = 0;
    virtual void installInnerEventFilter(QObject *filter) = 0;
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    void blockHighlighter(bool block);
    QWidget *innerEdit(QTextDocument **doc = 0) const;
    QTextDocument *innerDocument() const;
protected Q_SLOTS:
    void emitSelectionChanged();
    void setReadOnlyInternal(bool ro);
    void setModificationInternal(bool modified);
    void setHasSelection(bool hasSelection);
    void setCutAvailable(bool available);
    void setCopyAvailable(bool available);
    void setPasteAvailable(bool available);
    void setUndoAvailable(bool available);
    void setRedoAvailable(bool available);
    void setCursorPosition(const QPoint &pos);
    void setBuisy(bool buisy);
Q_SIGNALS:
    void readOnlyChanged(bool ro);
    void modificationChanged(bool modified);
    void selectionChanged();
    void hasSelectionChanged(bool hasSelection);
    void cutAvailableChanged(bool available);
    void copyAvailableChanged(bool available);
    void pasteAvailableChanged(bool available);
    void undoAvailableChanged(bool available);
    void redoAvailableChanged(bool available);
    void cursorPositionChanged(const QPoint &pos);
    void buisyChanged(bool buisy);
    void fileTypeChanged(BAbstractFileType *ft);
    void fileNameChanged(const QString &fn);
    void codecChanged(const QString &codecName);
    void loadingFinished(bool success);
    void savingFinished(bool success);
private:
    friend class BCodeEditPrivate;
    friend class BCodeEditorPrivate;
private:
    Q_DISABLE_COPY(BAbstractCodeEditorDocument)
};

#endif // BABSTRACTCODEEDITORDOCUMENT_H

