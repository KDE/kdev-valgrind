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

#include "callgrindview.h"

#include <QApplication>

#include <KIO/NetAccess>

#include <kdebug.h>
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "callgrindmodel.h"

namespace valgrind
{
CallgrindView::CallgrindView()
{
    connect(this, SIGNAL(activated(QModelIndex)), SLOT(openDocument(QModelIndex)));
}

CallgrindView::~CallgrindView() {}

void CallgrindView::setModel(valgrind::Model * m)
{
    QTreeView::setModel(m);
}

valgrind::Model * CallgrindView::model(void)
{
    return dynamic_cast<valgrind::Model *>(QTreeView::model());
}
}

#include "callgrindview.moc"
