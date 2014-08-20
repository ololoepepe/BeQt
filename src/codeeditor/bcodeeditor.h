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

#ifndef BCODEEDITOR_H
#define BCODEEDITOR_H

class BCodeEditorPrivate;

class BAbstractCodeEditorDocument;
class BAbstractDocumentDriver;
class BAbstractEditorModule;
class BAbstractFileType;
class BSpellChecker;

class QTextCodec;

#include "bcodeedit.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QFont>
#include <QList>
#include <QString>
#include <QStringList>
#include <QWidget>

/*============================================================================
================================ BCodeEditor =================================
============================================================================*/

class B_CODEEDITOR_EXPORT BCodeEditor : public QWidget, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(int asyncProcessingMinimumLength READ asyncProcessingMinimumLength
               WRITE setAsyncProcessingMinimumLength)
    Q_PROPERTY(bool autoCodecDetectionEnabled READ isAutoCodecDetectionEnabled WRITE setAutoCodecDetectionEnabled)
    Q_PROPERTY(bool bracketHighlightingEnabled READ isBracketHighlightingEnabled WRITE setBracketHighlightingEnabled)
    Q_PROPERTY(bool buisy READ isBuisy NOTIFY buisyChanged STORED false)
    Q_PROPERTY(QObject * closeHandler READ closeHandler CONSTANT)
    Q_PROPERTY(BAbstractCodeEditorDocument * currentDocument READ currentDocument NOTIFY currentDocumentChanged
               STORED false)
    Q_PROPERTY(QString currentDocumentFileName READ currentDocumentFileName NOTIFY currentDocumentFileNameChanged
               STORED false)
    Q_PROPERTY(bool currentDocumentModified READ isCurrentDocumentModified NOTIFY currentDocumentModificationChanged
               STORED false)
    Q_PROPERTY(QTextCodec * defaultCodec READ defaultCodec WRITE setDefaultCodec)
    Q_PROPERTY(QString defaultCodecName READ defaultCodecName WRITE setDefaultCodec NOTIFY defaultCodecChanged)
    Q_PROPERTY(QString defaultFileName READ defaultFileName WRITE setDefaultFileName)
    Q_PROPERTY(bool documentAvailable READ documentAvailable NOTIFY documentAvailableChanged STORED false)
    Q_PROPERTY(int documentType READ documentType STORED false)
    Q_PROPERTY(BAbstractDocumentDriver * driver READ driver WRITE setDriver)
    Q_PROPERTY(QObject * dropHandler READ dropHandler CONSTANT)
    Q_PROPERTY(bool editAutoIndentationEnabled READ isEditAutoIndentationEnabled WRITE setEditAutoIndentationEnabled)
    Q_PROPERTY(QFont editFont READ editFont WRITE setEditFont)
    Q_PROPERTY(QString editFontFamily READ editFontFamily WRITE setEditFontFamily STORED false)
    Q_PROPERTY(int editFontPointSize READ editFontPointSize WRITE setEditFontPointSize STORED false)
    Q_PROPERTY(int editLineLength READ editLineLength WRITE setEditLineLength)
    Q_PROPERTY(BCodeEdit::EditMode editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(QStringList fileHistory READ fileHistory WRITE setFileHistory NOTIFY fileHistoryChanged)
    Q_PROPERTY(QStringList fileNames READ fileNames STORED false)
    Q_PROPERTY(bool lineNumberWidgetVisible READ isLineNumberWidgetVisible WRITE setLineNumberWidgetVisible)
    Q_PROPERTY(BSpellChecker * spellChecker READ spellChecker WRITE setSpellChecker)
    Q_PROPERTY(int maximumFileSize READ maximumFileSize WRITE setMaximumFileSize)
    Q_PROPERTY(int maximumHistorySize READ maximumHistorySize WRITE setMaximumHistorySize)
    Q_PROPERTY(BAbstractFileType * preferredFileType READ preferredFileType WRITE setPreferredFileType)
    Q_PROPERTY(QString preferredFileTypeId READ preferredFileTypeId STORED false)
    B_DECLARE_PRIVATE(BCodeEditor)
public:
    enum StandardDocumentType
    {
        SimpleDocument = 1,
        StandardDocument
    };
    enum StandardModule
    {
        BookmarksModule = 1,
        EditModule,
        IndicatorsModule,
        OpenSaveModule,
        SearchModule
    };
public:
    typedef QList<BAbstractCodeEditorDocument *> DocumentList;
public:
    explicit BCodeEditor(QWidget *parent = 0);
    explicit BCodeEditor(int documentType, QWidget *parent = 0);
    ~BCodeEditor();
protected:
    explicit BCodeEditor(BCodeEditorPrivate &d, QWidget *parent = 0);
public:
    static BAbstractEditorModule *createStandardModule(StandardModule type, BCodeEditor *parent = 0);
public:
    void addFileType(BAbstractFileType *ft);
    void addModule(BAbstractEditorModule *mdl);
    void addModule(StandardModule type);
    int asyncProcessingMinimumLength() const;
    QObject *closeHandler() const;
    BAbstractCodeEditorDocument *currentDocument() const;
    QString currentDocumentFileName() const;
    QTextCodec *defaultCodec() const;
    QString defaultCodecName() const;
    QString defaultFileName() const;
    BAbstractCodeEditorDocument *document(const QString &fileName) const;
    bool documentAvailable() const;
    DocumentList documents() const;
    int documentType() const;
    BAbstractDocumentDriver *driver() const;
    QObject *dropHandler() const;
    QFont editFont() const;
    QString editFontFamily() const;
    int editFontPointSize() const;
    int editLineLength() const;
    BCodeEdit::EditMode editMode() const;
    BeQt::TabWidth editTabWidth() const;
    QStringList fileHistory() const;
    QStringList fileNames() const;
    BAbstractFileType *fileType(const QString &id) const;
    QList<BAbstractFileType *> fileTypes() const;
    bool isAutoCodecDetectionEnabled() const;
    bool isBracketHighlightingEnabled() const;
    bool isBuisy() const;
    bool isCurrentDocumentModified() const;
    bool isEditAutoIndentationEnabled() const;
    bool isLineNumberWidgetVisible() const;
    int maximumFileSize() const;
    int maximumHistorySize() const;
    void mergeWith(BCodeEditor *other);
    BAbstractEditorModule *module(const QString &name) const;
    BAbstractEditorModule *module(StandardModule type) const;
    QList<BAbstractEditorModule *> modules() const;
    BAbstractFileType *preferredFileType() const;
    QString preferredFileTypeId() const;
    void removeFileType(BAbstractFileType *ft);
    void removeFileType(const QString &id);
    void removeModule(BAbstractEditorModule *mdl);
    void removeModule(const QString &name);
    void setAsyncProcessingMinimumLength(int length);
    void setAutoCodecDetectionEnabled(bool b);
    void setBracketHighlightingEnabled(bool b);
    void setCurrentDocument(BAbstractCodeEditorDocument *doc);
    void setDefaultCodec(QTextCodec *codec);
    void setDefaultCodec(const QString &codecName);
    void setDefaultFileName(const QString &fileName);
    void setDocumentType(int type);
    void setDriver(BAbstractDocumentDriver *drv);
    void setEditAutoIndentationEnabled(bool enabled);
    void setEditFont(const QFont &fnt);
    void setEditFontFamily(const QString &family);
    void setEditFontPointSize(int pointSize);
    void setEditLineLength(int ll);
    void setEditMode(BCodeEdit::EditMode mode);
    void setEditTabWidth(BeQt::TabWidth tw);
    void setFileHistory(const QStringList &list);
    void setFileTypes(const QList<BAbstractFileType *> &list);
    void setLineNumberWidgetVisible(bool b);
    void setMaximumFileSize(int sz);
    void setMaximumHistorySize(int count);
    void setModules(const QList<BAbstractEditorModule *> &list);
    void setPreferredFileType(BAbstractFileType *ft);
    void setPreferredFileType(const QString &id);
    void setSpellChecker(BSpellChecker *sc);
    BSpellChecker *spellChecker() const;
    bool waitForAllDocumentsProcessed(int msecs = 30 * BeQt::Second) const;
public Q_SLOTS:
    BAbstractCodeEditorDocument *addDocument(const QString &fileName = QString());
    BAbstractCodeEditorDocument *addDocument(const QString &fileName, const QString &text);
    bool closeAllDocuments(bool save = true);
    bool closeCurrentDocument();
    void insertTextIntoCurrentDocument(const QString &text);
    BAbstractCodeEditorDocument *openDocument(const QString &fileName, QTextCodec *codec = 0);
    DocumentList openDocuments();
    DocumentList openDocuments(const QStringList &fileNames, QTextCodec *codec = 0);
    bool reopenCurrentDocument(QTextCodec *codec = 0);
    bool reopenCurrentDocument(const QString &codecName);
    bool saveAllDocuments();
    bool saveCurrentDocument();
    bool saveCurrentDocumentAs();
    bool saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec = 0);
    void setCurrentDocumentText(const QString &text);
protected:
    virtual BAbstractCodeEditorDocument *createDocument(int type, BCodeEditor *editor) const;
    virtual void showClosingMessage(QWidget *parent);
Q_SIGNALS:
    void allDocumentsProcessed();
    bool buisyChanged(bool buisy);
    void currentDocumentChanged(BAbstractCodeEditorDocument *doc);
    void currentDocumentFileNameChanged(const QString &fileName);
    void currentDocumentModificationChanged(bool modified);
    void defaultCodecChanged(const QString &codecName);
    void documentAboutToBeAdded(BAbstractCodeEditorDocument *doc);
    void documentAboutToBeRemoved(BAbstractCodeEditorDocument *doc);
    void documentAdded(BAbstractCodeEditorDocument *doc);
    void documentAvailableChanged(bool available);
    void documentRemoved(const QString &fileName);
    void editModeChanged(BCodeEdit::EditMode mode);
    void fileHistoryChanged(const QStringList &history);
    void fileTypesChanged();
private:
    Q_DISABLE_COPY(BCodeEditor)
};

#endif // BCODEEDITOR_H
