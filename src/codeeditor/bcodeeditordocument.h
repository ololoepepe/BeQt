#ifndef BCODEEDITORDOCUMENT_H
#define BCODEEDITORDOCUMENT_H

class BCodeEditorDocumentPrivate;
class BSplittedLinesDialog;
class BCodeEditor;

class QWidget;
class QPoint;
class QFont;
class QString;

#include "babstractcodeeditordocument.h"
#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QTextDocument>

/*============================================================================
================================ BCodeEditorDocument =========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorDocument : public BAbstractCodeEditorDocument
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCodeEditorDocument)
public:
    explicit BCodeEditorDocument(BCodeEditor *editor, QWidget *parent = 0);
    ~BCodeEditorDocument();
protected:
    explicit BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent = 0);
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
    QFont editFont() const;
    BeQt::TabWidth editTabWidth() const;
    bool lineNumberWidgetVisible() const;
    QString text(bool full = false) const;
    QString selectedText(bool full = false) const;
    QPoint selectionStart() const;
    QPoint selectionEnd() const;
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditLineLength(int ll);
    void setSplittedLinesDialog(BSplittedLinesDialog *dlg);
    BCodeEdit::EditMode editMode() const;
    int editLineLength() const;
    BSplittedLinesDialog *splittedLinesDialog() const;
public Q_SLOTS:
    void switchMode();
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
    void installDropHandler(QObject *handler);
    void installInnerEventFilter(QObject *filter);
Q_SIGNALS:
    void editModeChanged(BCodeEdit::EditMode mode);
    void lineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void linesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
private:
    Q_DISABLE_COPY(BCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
