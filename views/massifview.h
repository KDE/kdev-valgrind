/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>

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

#ifndef MASSIFVIEW_H
#define MASSIFVIEW_H

#include <QTabWidget>
#include <QTreeView>

#include "iview.h"

namespace valgrind
{

    class MassifView : public QTabWidget, public valgrind::IView
    {
        Q_OBJECT

    public:
        MassifView();
        ~MassifView();
        void setModel( valgrind::Model * m );
        valgrind::Model * model( void );
        QWidget * widget( void );

    private Q_SLOTS:
        void openDocument(const QModelIndex& index);

    private:
        QTreeView m_tree;
        QTreeView m_tree_test;
        valgrind::Model * m_model;
    };
}
#endif // MASSIFVIEW_H
