#ifndef BHELPWIDGET_H
#define BHELPWIDGET_H

class QString;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTextBrowser;
class QEvent;
class QCloseEvent;

#include <QWidget>

class BHelpWidget : public QWidget
{
    Q_OBJECT
public:
    BHelpWidget(const QString &settingsGroup, const QString &home, const QString &url, QWidget *parent = 0);
protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    const QString _m_CSettingsGroup;
    //
    QVBoxLayout *_m_vlt;
      QHBoxLayout *_m_hltActions;
        QPushButton *_m_btnHome;
        QPushButton *_m_btnBack;
        QPushButton *_m_btnForward;
        //stretch
        QPushButton *_m_btnClose;
      QTextBrowser *_m_tbsr;
    //
    void _m_retranslateUi();
    void _m_loadSettings();
    void _m_saveSettings();
private slots:
    void _m_updateCaption();
};

#endif // BHELPWIDGET_H
