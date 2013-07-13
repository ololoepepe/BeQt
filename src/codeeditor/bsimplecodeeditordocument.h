#ifndef BSIMPLECODEEDITORDOCUMENT_H
#define BSIMPLECODEEDITORDOCUMENT_H

class BSimpleCodeEditorDocumentPrivate;
class BCodeEditor;

class QWidget;
class QPoint;
class QFont;
class QString;
class QColor;

#include "babstractcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QTextDocument>

/*============================================================================
================================ BSimpleCodeEditorDocument ===================
============================================================================*/

class B_CODEEDITOR_EXPORT BSimpleCodeEditorDocument : public BAbstractCodeEditorDocument
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSimpleCodeEditorDocument)
public:
    explicit BSimpleCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BSimpleCodeEditorDocument();
protected:
    explicit BSimpleCodeEditorDocument(BSimpleCodeEditorDocumentPrivate &d, QWidget *parent = 0);
public:
    void setReadOnly(bool ro);
    void setModification(bool modified);
    void setEditFont(const QFont &fnt);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setLineNumberWidgetVisible(bool b);
    void setCurrentLineHighlightingEnabled(bool b);
    void setHighlightedLineColor(const QColor &c);
    bool findNext(const QString &txt, QTextDocument::FindFlags flags = 0, bool cyclic = true);
    bool replaceNext(const QString &newText);
    int replaceInSelection(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    int replaceInDocument(const QString &txt, const QString &newText, Qt::CaseSensitivity cs);
    bool isReadOnly() const;
    bool isModified() const;
    bool hasSelection() const;
    bool isCutAvailable() const;
    bool isCopyAvailable() const;
    bool isPasteAvailable() const;
    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    QFont editFont() const;
    BeQt::TabWidth editTabWidth() const;
    bool lineNumberWidgetVisible() const;
    bool currentLineHighlightingEnabled() const;
    QColor highlightedLineColor() const;
    QPoint cursorPosition() const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    QPoint selectionStart() const;
    QPoint selectionEnd() const;
protected:
    QWidget *createEdit(QTextDocument **doc = 0);
    void setFocusImplementation();
    void activateWindowImplementation();
    void setTextImplementation(const QString &txt, int asyncIfLongerThan = 100 * BeQt::Kilobyte);
    void insertTextImplementation(const QString &txt);
    void moveCursorImplementation(const QPoint &pos);
    void selectTextImplementation(const QPoint &start, const QPoint &end);
    void selectTextImplementation(int start, int end);
    void selectLinesImplementation(int firstLine, int lastLine);
    void selectAllImplementation();
    void deselectTextImplementation();
    void cutImplementation();
    void copyImplementation();
    void pasteImplementation();
    void deleteSelectionImplementation();
    void undoImplementation();
    void redoImplementation();
    void clearUndoRedoStacks(QTextDocument::Stacks historyToClear = QTextDocument::UndoAndRedoStacks);
    void highlightBrackets();
    void installDropHandler(QObject *handler);
    void installInnerEventFilter(QObject *filter);
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
