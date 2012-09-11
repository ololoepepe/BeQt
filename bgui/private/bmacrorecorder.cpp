#include "bmacrorecorder.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QKeySequence>
#include <QWidget>

#include <QDebug>

QString constructMessage(int key, Qt::KeyboardModifiers modifiers, const QString &text)
{
    if ( !text.isEmpty() && text.at(0).isPrint() && !text.at(0).isSpace() )
        return text;
    else
        return QKeySequence(key | modifiers).toString(QKeySequence::NativeText);
}

//

BMacroRecorder::BMacroRecorder(QObject *parent) :
    QObject(parent)
{
    _m_started = false;
}

BMacroRecorder::~BMacroRecorder()
{
    _m_started = false;
    clear();
}

//

void BMacroRecorder::startRecording()
{
    if (_m_started)
        return;
    clear();
    _m_started = true;
}

void BMacroRecorder::handleKeyPress(QKeyEvent *event)
{
    if (!_m_started || !event || event->type() != QEvent::KeyPress)
        return;
    int k = event->key();
    if (Qt::Key_Control == k || Qt::Key_Alt == k || Qt::Key_Shift == k)
        return;
    Qt::KeyboardModifiers m = event->modifiers();
    QString t = event->text();
    _m_events << new QKeyEvent(QEvent::KeyPress, k, m, t);
    emit keyPressAdded( constructMessage(k, m, t) );
}

void BMacroRecorder::stopRecording()
{
    _m_started = false;
}

void BMacroRecorder::clear()
{
    if (_m_started)
        return;
    for (int i = 0; i < _m_events.size(); ++i)
        delete _m_events.at(i);
    _m_events.clear();
    emit cleared();
}

bool BMacroRecorder::loadMacro(const QString &fileName)
{
    if (_m_started)
        return false;
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return false;
    clear();
    QTextStream in(&f);
    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        int i = 0;
        QString s;
        QStringList sl;
        while ( i < line.length() )
        {
            if (line.at(i) == ' ')
            {
                if ( s.isEmpty() )
                    sl << " ";
                else
                    sl << s;
                s.clear();
            }
            else
            {
                s += line.at(i);
            }
            ++i;
        }
        if ( !s.isEmpty() )
            sl << s;
        if (sl.size() < 2)
            continue;
        bool ok = false;
        int key = sl.takeFirst().toInt(&ok);
        if (!ok)
            continue;
        Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifier>( sl.takeFirst().toInt(&ok) );
        if (!ok)
            continue;
        QString text;
        if ( !sl.isEmpty() )
            text = sl.join(" ");
        _m_events << new QKeyEvent(QEvent::KeyPress, key, modifiers, text);
        emit keyPressAdded( constructMessage( key, modifiers, text) );
    }
    f.close();
    return true;
}

bool BMacroRecorder::saveMacro(const QString &fileName)
{
    QFile f(fileName);
    if ( !f.open(QFile::WriteOnly) )
        return false;
    QTextStream out(&f);
    for (int i = 0; i < _m_events.size(); ++i)
    {
        QKeyEvent *e = _m_events.at(i);
        if (!e)
            continue;
        QString s = QString::number( e->key() ) + " " + QString::number( e->modifiers() );
        if ( !e->text().isEmpty() )
            s += " " + e->text();
        out << s << "\n";
    }
    f.close();
    return true;
}

void BMacroRecorder::applyMacro()
{
    QWidget *wnd = QApplication::activeWindow();
    if (!wnd)
        return;
    for (int i = 0; i < _m_events.size(); ++i)
    {
        QKeyEvent *e = _m_events.at(i);
        if (!e)
            continue;
        QWidget *wgt = QApplication::focusWidget();
        QApplication::postEvent(wgt ? wgt : wnd,
                                new QKeyEvent( QEvent::KeyPress, e->key(), e->modifiers(), e->text() ),
                                Qt::LowEventPriority);
        QApplication::processEvents();
    }
}

bool BMacroRecorder::isStarted() const
{
    return _m_started;
}

bool BMacroRecorder::isEmpty() const
{
    return _m_events.isEmpty();
}
