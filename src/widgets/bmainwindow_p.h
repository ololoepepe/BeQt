#ifndef BMAINWINDOW_P_H
#define BMAINWINDOW_P_H

class BMainWindowPrivate;

class QString;
class QAction;
class QMenu;

#include "bmainwindow.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QApplication>

class B_WIDGETS_EXPORT BMainWindowPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BMainWindowPrivateObject(BMainWindowPrivate *p);
    ~BMainWindowPrivateObject();
    //
    BMainWindowPrivate *const _m_p;
    //
public slots:
    void languageChanged();
private:
    Q_DISABLE_COPY(BMainWindowPrivateObject)
};

class B_WIDGETS_EXPORT BMainWindowPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BMainWindow)
    Q_DECLARE_TR_FUNCTIONS(BMainWindow)
public:
    enum Action
    {
        HomepageAction,
        ContentsAction,
        ContextualAction,
        WhatsThisAction,
        AboutAction
    };
    enum Menu
    {
        HelpMenu
    };
    //
    BMainWindowPrivate(BMainWindow *q, const QString &sg);
    ~BMainWindowPrivate();
    //
    BMainWindowPrivateObject *const _m_o;
    const QString SettingsGroup;
    //
    void languageChanged();
    void retranslateAction(QAction *action, Action type) const;
    void retranslateMenu(QMenu *menu, Menu type) const;
private:
    Q_DISABLE_COPY(BMainWindowPrivate)
    //
    friend class BMainWindowPrivateObject;
};

#endif // BMAINWINDOW_P_H
