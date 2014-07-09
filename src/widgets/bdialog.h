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

#ifndef BDIALOG_H
#define BDIALOG_H

class BDialogPrivate;

class QAbstractButton;
class QDialogButtonBox;
class QPushButton;
class QString;
class QWidget;

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QDialog>
#include <QDialogButtonBox>
#include <QList>

/*============================================================================
================================ BDialog =====================================
============================================================================*/

class B_WIDGETS_EXPORT BDialog : public QDialog, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BDialog)
public:
    explicit BDialog(QWidget *parent = 0);
    explicit BDialog(QDialogButtonBox::StandardButtons buttons, QWidget *parent = 0);
    ~BDialog();
protected:
    explicit BDialog(BDialogPrivate &d, QWidget *parent = 0);
public:
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const char *method = 0);
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QObject *object,
                   const char *method);
    void addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const char *method = 0);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QObject *object,
                           const char *method);
    QPushButton *addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const char *method = 0);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const QObject *object, const char *method);
    QPushButton *addButton(QDialogButtonBox::StandardButton button, const QList<BeQt::Target> &targets);
    QPushButton *button(QDialogButtonBox::StandardButton which) const;
    QDialogButtonBox *buttonBox() const;
    QDialogButtonBox::ButtonRole buttonRole(QAbstractButton *button) const;
    QList<QAbstractButton *> buttons() const;
    bool centerButtons() const;
    void removeButton(QAbstractButton *button);
    void setCenterButtons(bool center);
    void setStandardButtons(QDialogButtonBox::StandardButtons buttons);
    void setStretchEnabled(bool b);
    void setWidget(QWidget *w);
    QDialogButtonBox::StandardButton standardButton(QAbstractButton *button) const;
    QDialogButtonBox::StandardButtons standardButtons() const;
    bool stretchEnabled() const;
    QWidget *widget() const;
private:
    Q_DISABLE_COPY(BDialog)
};

#endif // BDIALOG_H
