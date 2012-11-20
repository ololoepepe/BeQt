#ifndef BSEARCHDIALOG_H
#define BSEARCHDIALOG_H

class BSearchDialogPrivate;
class BCodeEditorDocument;

class QWidget;
class QString;
class QByteArray;
class QStringList;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QDialog>

/*============================================================================
================================ Search Dialog
============================================================================*/

class B_CODEEDITOR_EXPORT BSearchDialog : public QDialog, public BBase
{
    B_DECLARE_PRIVATE(BSearchDialog)
    Q_OBJECT
public:
    explicit BSearchDialog(QWidget *parent = 0);
    explicit BSearchDialog(bool replaceEnabled, QWidget *parent = 0);
    ~BSearchDialog();
    //
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setMatchWholeWords(bool enabled);
    void setBackwardOrder(bool enabled);
    void setCyclicSearch(bool enabled);
    void setSearchHistory(const QStringList &list);
    void setReplaceHistory(const QStringList &list);
    void setMaximumHistorySize(int sz);
    void setReplaceEnabled(bool enabled);
    void setDocument(BCodeEditorDocument *doc);
    void restoreState(const QByteArray &ba);
    Qt::CaseSensitivity caseSensitivity() const;
    bool matchWholeWords() const;
    bool backwardOrder() const;
    bool cyclicSearch() const;
    QStringList searchHistory() const;
    QStringList replaceHistory() const;
    int maximumHistorySize() const;
    bool replaceEnabled() const;
    BCodeEditorDocument *document() const;
    bool findNextAvailable() const;
    bool replaceNextAvailable() const;
    QByteArray saveState() const;
public slots:
    void findNext();
    void replaceNext();
signals:
    void findNextAvailableChanged(bool available);
    void replaceNextAvailableChanged(bool available);
    void textFound(bool found, const QString &text);
    void textReplaced(int count, const QString &oldText, const QString &newText);
protected:
    BSearchDialog(BSearchDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BSearchDialog)
};

#endif // BSEARCHDIALOG_H

