#ifndef BAPPLICATION_H
#define BAPPLICATION_H

class BApplicationPrivate;

class QIcon;
class QString;

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>

#include <QObject>

#if defined(bApp)
#   undef bApp
#endif
#define bApp ( static_cast<BApplication *>( BCoreApplication::instance() ) )

class B_WIDGETS_EXPORT BApplication : public BCoreApplication
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BApplication)
    B_DECLARE_SELF(BApplication)
    B_DECLARE_PRIVATE_S(BApplication)
public:
    static QIcon beqtIcon(const QString &fileName);
    //
    explicit BApplication( const AppOptions &options = AppOptions() );
    ~BApplication();
public slots:
    void showAbout();
protected:
    BApplication(BApplicationPrivate &d);
private:
    Q_DISABLE_COPY(BApplication)
};

#endif // BAPPLICATION_H
