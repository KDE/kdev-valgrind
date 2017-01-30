/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>


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

#ifndef CACHEGRINDVIEW_H
#define CACHEGRINDVIEW_H

#include <QTreeView>

#include "interfaces/iview.h"

namespace valgrind
{
class CachegrindView : public QTreeView, public IView
{
    Q_OBJECT

public:
    CachegrindView();
    ~CachegrindView() override;

    using QTreeView::setModel;
    using QTreeView::model;

    void setModel(IModel* m) override;

protected:
  void MousePressEvent(QMouseEvent *event);

private:
    void openDocument(const QModelIndex& index);
};
}
#endif // CACHEGRINDVIEW_H
