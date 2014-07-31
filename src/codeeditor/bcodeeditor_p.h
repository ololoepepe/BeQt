/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef BCODEEDITOR_P_H
#define BCODEEDITOR_P_H

class BSelectDocumentsDialogPrivate;
class BSplittedLinesDialogPrivate;

class BAbstractCodeEditorDocument;
class BAbstractDocumentDriver;
class BAbstractEditorModule;
class BAbstractFileType;
class BCodeEditorDocument;
class BSpellChecker;

class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QEvent;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPoint;
class QPushButton;
class QTabWidget;
class QTextCodec;

#include "bcodeeditor.h"

#include "bcodeedit.h"

#include <BeQtCore/BBase>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/private/bbase_p.h>

#include <QDialog>
#include <QFont>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BSelectDocumentsDialog ======================
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSelectDocumentsDialog)
public:
    explicit BSelectDocumentsDialog(const QList<BAbstractCodeEditorDocument *> &documents, QWidget *parent = 0);
    ~BSelectDocumentsDialog();
protected:
    explicit BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent = 0);
public:
    int decision() const;
    QList<BAbstractCodeEditorDocument *> selectedDocuments() const;
private:
    Q_DISABLE_COPY(BSelectDocumentsDialog)
};

/*============================================================================
================================ BSelectDocumentsDialogPrivate ===============
============================================================================*/

class B_CODEEDITOR_EXPORT BSelectDocumentsDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSelectDocumentsDialog)
public:
    const QList<BAbstractCodeEditorDocument *> Documents;
public:
    int decision;
    QDialogButtonBox *dlgbbox;
    QLabel *lblInformativeTextUpper;
    QLabel *lblInformativeTextLower;
    QLabel *lblText;
    QListWidget *lstwgt;
public:
    explicit BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q,
                                           const QList<BAbstractCodeEditorDocument *> &list);
    ~BSelectDocumentsDialogPrivate();
public:
    void init();
public Q_SLOTS:
    void dlgbboxClicked(QAbstractButton *button);
private:
    Q_DISABLE_COPY(BSelectDocumentsDialogPrivate)
};

/*============================================================================
================================ BSplittedLinesDialog ========================
============================================================================*/

class B_CODEEDITOR_EXPORT BSplittedLinesDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSplittedLinesDialog)
public:
    explicit BSplittedLinesDialog(const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength,
                                  QWidget *parent = 0);
    ~BSplittedLinesDialog();
protected:
    explicit BSplittedLinesDialog(BSplittedLinesDialogPrivate &d, QWidget *parent = 0);
Q_SIGNALS:
    void gotoLine(const QPoint &pos);
    void selectLines(const QPoint &start, const QPoint &end);
private:
    Q_DISABLE_COPY(BSplittedLinesDialog)
};

/*============================================================================
================================ BSplittedLinesDialogPrivate =================
============================================================================*/

class B_CODEEDITOR_EXPORT BSplittedLinesDialogPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSplittedLinesDialog)
public:
    const int LineLength;
    const QList<BCodeEdit::SplittedLinesRange> Ranges;
public:
    QPushButton *btnGoto;
    QCheckBox *cboxSelect;
    QDialogButtonBox *dlgbbox;
    QLabel *lbl;
    QListWidget *lstwgt;
public:
    explicit BSplittedLinesDialogPrivate(BSplittedLinesDialog *q,
                                         const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength);
    ~BSplittedLinesDialogPrivate();
public:
    static QListWidgetItem *createListWidgetItem(const BCodeEdit::SplittedLinesRange &range, int lineLength);
public:
    void init();
public Q_SLOTS:
    void btnGotoClicked();
    void lstwgtItemDoubleClicked(QListWidgetItem *item);
private:
    Q_DISABLE_COPY(BSplittedLinesDialogPrivate)
};

/*============================================================================
================================ BDropHandler ================================
============================================================================*/

class B_CODEEDITOR_EXPORT BDropHandler : public QObject
{
    Q_OBJECT
private:
    BCodeEditor *const Editor;
public:
    explicit BDropHandler(BCodeEditor *parent);
    ~BDropHandler();
public:
    bool eventFilter(QObject *o, QEvent *e);
private:
    Q_DISABLE_COPY(BDropHandler)
};

/*============================================================================
================================ BCloseHandler ===============================
============================================================================*/

class B_CODEEDITOR_EXPORT BCloseHandler : public QObject
{
    Q_OBJECT
private:
    BCodeEditorPrivate *const Editor;
private:
    QObject *lastSender;
public:
    explicit BCloseHandler(BCodeEditorPrivate *parent);
    ~BCloseHandler();
public:
    bool eventFilter(QObject *o, QEvent *e);
public Q_SLOTS:
    void processingFinished();
private:
    Q_DISABLE_COPY(BCloseHandler)
};

/*============================================================================
================================ BCodeEditorPrivate ==========================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditorPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCodeEditor)
public:
    enum Operation
    {
        AddOperation = 1,
        OpenOperation,
        SaveOperation
    };
public:
    int asyncMin;
    bool autoCodecDetection;
    bool bracketsHighlighting;
    BCloseHandler *closeHandler;
    QList<BAbstractCodeEditorDocument *> closingDocuments;
    QTextCodec *defaultCodec;
    QString defaultFN;
    int docType;
    BAbstractCodeEditorDocument *document;
    BAbstractDocumentDriver *driver;
    BDropHandler *dropHandler;
    bool editAutoIndentation;
    QFont editFont;
    int editLineLength;
    BCodeEdit::EditMode editMode;
    BeQt::TabWidth editTabWidth;
    QStringList fileHistory;
    QMap<QString, BAbstractFileType *> fileTypes;
    bool lineNumberVisible;
    int maxHistoryCount;
    int maximumFileSize;
    QMap<QString, BAbstractEditorModule *> modules;
    QMap<BAbstractCodeEditorDocument *, QString> openingDocuments;
    BAbstractFileType *preferredFileType;
    QList<BAbstractCodeEditorDocument *> processedDocuments;
    QMap<BAbstractCodeEditorDocument *, QString> savingDocuments;
    BSpellChecker *spellChecker;
    QTabWidget *twgt;
public:
    explicit BCodeEditorPrivate(BCodeEditor *q);
    ~BCodeEditorPrivate();
public:
    static QString createFileName(const QString &fileName, const QString &defaultName,
                                  const QStringList &existingNames);
    static QString defaultFileName();
public:
    void addDocument(BAbstractCodeEditorDocument *doc);
    void alreadyOpenedMessage(const QString &fileName);
    void appendFileHistory(const QString &fileName, const QString &oldFileName = QString());

    bool closeAllDocuments(bool save = true);
    bool closeDocument(BAbstractCodeEditorDocument *doc);
    int closeModifiedMessage(const QString &fileName);
    BAbstractCodeEditorDocument *createDocument(const QString &fileName = QString(), const QString &text = QString());
    BSplittedLinesDialog *createSplittedLinesDialog(BCodeEditorDocument *doc,
                                                    const QList<BCodeEdit::SplittedLinesRange> ranges);
    void emitCurrentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void emitCurrentDocumentFileNameChanged(const QString &fileName);
    void emitCurrentDocumentModificationChanged(bool modified);
    void emitDefaultCodecChanged(const QString &codecName);
    void emitDocumentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    void emitDocumentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    void emitDocumentAdded(BAbstractCodeEditorDocument *doc);
    void emitDocumentRemoved(const QString &fileName);
    void emitEditModeChanged(BCodeEdit::EditMode mode);
    void emitFileHistoryChanged(const QStringList &list);
    void emitFileTypesChanged();
    bool eventFilter(QObject *o, QEvent *e);
    void failedToOpenMessage(const QString &fileName, const QString &info = QString());
    void failedToSaveMessage(const QString &fileName, const QString &newFileName = QString());
    bool findDocument(const QString &fileName);
    void init();
    bool isDefaultFileName(const QString &fileName) const;
    BAbstractCodeEditorDocument *openDocument(QString fileName, QTextCodec *codec = 0);
    void removeDocument(BAbstractCodeEditorDocument *doc);
    bool reopenDocument(BAbstractCodeEditorDocument *doc, QTextCodec *codec = 0);
    int reopenModifiedMessage(const QString &fileName);

    bool saveDocument(BAbstractCodeEditorDocument *doc, const QString &newFileName = QString(), QTextCodec *codec = 0,
                      BeQt::LineFeed lineFeed = BeQt::DefaultLineFeed);
    bool saveDocuments(const QList<BAbstractCodeEditorDocument *> &list);
    BAbstractFileType *selectDocumentFileType(BAbstractCodeEditorDocument *doc);
    void setDriverEditor(BAbstractDocumentDriver *drv, BCodeEditor *edr);
    void setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr);
    void showClosingMessage(QWidget *parent);
    bool tryAddFileType(BAbstractFileType *ft);
    bool tryCloseDocument(BAbstractCodeEditorDocument *doc);
    bool tryRemoveFileType(const QString &id);
    void updateDocumentTab(BAbstractCodeEditorDocument *doc);
public Q_SLOTS:
    void documentBuisyChanged(bool buisy);
    void documentCodecChanged(const QString &codecName);
    void documentCopyAvailableChanged(bool available);
    void documentCursorPositionChanged(const QPoint &pos);
    void documentCutAvailableChanged(bool available);
    void documentEditModeChanged(BCodeEdit::EditMode mode);
    void documentFileNameChanged(const QString &fn);
    void documentFileTypeChanged(BAbstractFileType *ft);
    void documentHasSelectionChanged(bool hasSelection);
    void documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange);
    void documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges);
    void documentLoadingFinished(bool success);
    void documentModificationChanged(bool modified);
    void documentPasteAvailableChanged(bool available);
    void documentReadOnlyChanged(bool ro);
    void documentRedoAvailableChanged(bool available);
    void documentSavingFinished(bool success);
    void documentSelectionChanged();
    void documentUndoAvailableChanged(bool available);
    void twgtCurrentChanged(int index);
    void twgtTabCloseRequested(int index);
private:
    Q_DISABLE_COPY(BCodeEditorPrivate)
    friend class BCloseHandler;
};

#endif // BCODEEDITOR_P_H
