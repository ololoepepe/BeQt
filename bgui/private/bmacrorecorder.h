#ifndef BMACRORECORDER_H
#define BMACRORECORDER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QKeyEvent>

class BMacroRecorder : public QObject
{
    Q_OBJECT
public:
    explicit BMacroRecorder(QObject *parent = 0);
    ~BMacroRecorder();
    //
    void startRecording();
    void handleKeyPress(QKeyEvent *event);
    void stopRecording();
    void clear();
    bool loadMacro(const QString &fileName);
    bool saveMacro(const QString &fileName);
    void applyMacro();
    bool isStarted() const;
    bool isEmpty() const;
private:
    bool _m_started;
    QList<QKeyEvent *> _m_events;
signals:
    void keyPressAdded(const QString &message);
    void cleared();
};

#endif // BMACRORECORDER_H
