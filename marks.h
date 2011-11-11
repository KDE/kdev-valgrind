/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _MARKS_H_
#define _MARKS_H_

#include <QObject>

namespace valgrind
{
class Model;
class Plugin;

/*
 * Interactions between the file editor and Valgrind's output
 */
class Marks : public QObject
{
    Q_OBJECT

public:

    Marks(valgrind::Plugin *plugin);

    virtual ~Marks();

private slots:
    /*
     * Sets the new model
     * Clean the previous marks if necessary
     */
    void newModel(valgrind::Model* model);

    void modelChanged();
private:
    valgrind::Plugin  *m_plugin;
    valgrind::Model   *m_model;
};

}



#endif /* _MARKS_H_ */
