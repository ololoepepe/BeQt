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
    bool loadMacro(const QString &fileName);
    bool saveMacro(const QString &fileName);
    void applyMacro();
    bool isStarted() const;
    bool isEmpty() const;
public slots:
    void clear();
private:
    static QString _m_constructMessage(int key, Qt::KeyboardModifiers modifiers, const QString &text);
    //
    bool _m_started;
    QList<QKeyEvent *> _m_events;
signals:
    void keyPressAdded(const QString &message);
    void cleared();
    void macroAvailableChanged(bool available);
};

#endif // BMACRORECORDER_H
