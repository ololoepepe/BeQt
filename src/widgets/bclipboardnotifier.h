/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
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

#ifndef BCLIPBOARDNOTIFIER_H
#define BCLIPBOARDNOTIFIER_H

class BClipboardNotifierPrivate;

class QWidget;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBaseObject>

#include <QObject>

/*============================================================================
================================ BClipboardNotifier ==========================
============================================================================*/

class B_WIDGETS_EXPORT BClipboardNotifier : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BClipboardNotifier)
public:
    explicit BClipboardNotifier();
    ~BClipboardNotifier();
public:
    static BClipboardNotifier *instance();
public:
    bool textDataAvailable() const;
Q_SIGNALS:
    void textDataAvailableChanged(bool available);
protected:
    static BClipboardNotifier *_m_self;
private:
    Q_DISABLE_COPY(BClipboardNotifier)
};

#endif // BCLIPBOARDNOTIFIER_H
