#ifndef BSIMPLECODEEDITORDOCUMENT_H
#define BSIMPLECODEEDITORDOCUMENT_H

class BSimpleCodeEditorDocumentPrivate;
class BCodeEditor;

class QWidget;
class QPoint;
class QFont;
class QString;

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
protected:
    QWidget *createEdit(QTextDocument **doc = 0);
    void setFocusImplementation();
    void activateWindowImplementation();
    void setTextImplementation(const QString &txt);
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
    void installDropHandler(QObject *handler);
    void installInnerEventFilter(QObject *filter);
private:
    Q_DISABLE_COPY(BSimpleCodeEditorDocument)
};

#endif // BCODEEDITORDOCUMENT_H
