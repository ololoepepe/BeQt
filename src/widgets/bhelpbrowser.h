#ifndef BHELPBROWSER_H
#define BHELPBROWSER_H

class BHelpBrowserPrivate;

class QString;
class QStringList;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>

/*============================================================================
================================ Help Browser
============================================================================*/

class B_WIDGETS_EXPORT BHelpBrowser : public QWidget, public BBase
{
    B_DECLARE_PRIVATE(BHelpBrowser)
    Q_OBJECT
public:
    static void clearSearchCache();
    //
    explicit BHelpBrowser(QWidget *parent = 0);
    explicit BHelpBrowser(const QStringList &searchPaths, QWidget *parent = 0);
    explicit BHelpBrowser(const QString &file, QWidget *parent = 0);
    BHelpBrowser(const QStringList &searchPaths, const QString &file, QWidget *parent = 0);
    BHelpBrowser(const QString &index, const QString &file, QWidget *parent = 0);
    BHelpBrowser(const QStringList &searchPaths, const QString &index, const QString &file, QWidget *parent = 0);
    ~BHelpBrowser();
    //
    void setSearchPaths(const QStringList &paths);
    void setFile(const QString &file);
protected:
    BHelpBrowser(BHelpBrowserPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BHelpBrowser)
};

#endif // BHELPBROWSER_H
