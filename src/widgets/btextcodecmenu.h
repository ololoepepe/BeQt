#ifndef BTEXTCODECMENU_H
#define BTEXTCODECMENU_H

class BTextCodecMenuPrivate;

class QTextCodec;
class QString;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QMenu>

/*============================================================================
================================ BTextCodecMenu ==============================
============================================================================*/

class B_WIDGETS_EXPORT BTextCodecMenu : public QMenu, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTextCodecMenu)
public:
    enum Style
    {
        StructuredStyle = 0,
        PlainStyle
    };
public:
    explicit BTextCodecMenu(QWidget *parent = 0);
    explicit BTextCodecMenu(Style s, QWidget *parent = 0);
    ~BTextCodecMenu();
protected:
    explicit BTextCodecMenu(BTextCodecMenuPrivate &d, QWidget *parent = 0);
public:
    void setMapping(const QObject *receiver, const char *method);
    Style style() const;
    QTextCodec *selectedCodec() const;
    QString selectedCodecName() const;
private:
    Q_DISABLE_COPY(BTextCodecMenu)
};

#endif // BTEXTCODECMENU_H
