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

#ifndef CALLGRINDVIEW_H
#define CALLGRINDVIEW_H

#include <QWidget>
#include <QItemSelection>

#include "modelitem.h"

#include "interfaces/iview.h"


class QResizeEvent;
class QSortFilterProxyModel;

namespace valgrind
{
namespace Ui
{
class CallgrindView;
}

class CallgrindModel;

class CallgrindView : public QWidget, public IView
{
    Q_OBJECT

public:
    explicit CallgrindView(QWidget* parent = nullptr);
    ~CallgrindView() override;

    void setModel(Model * m) override;
    Model * model(void) override;

    void WidgetContainerResizeEvent(QResizeEvent * event) override;

public slots:
    void percentInformationClicked(bool enable);

    void selectionOnFctListChanged(const QItemSelection &, const QItemSelection &);
    void selectionOnCallerListChanged(const QItemSelection &, const QItemSelection &);

private:
    Ui::CallgrindView                         *ui;

    CallgrindModel                            *m_model;

    QSortFilterProxyModel                     *m_callerProxyModel;
    QSortFilterProxyModel                     *m_calleeProxyModel;

    CallgrindCallstackItem                    *m_functionListSelectedItem;

    CallgrindCallstackItem::numberDisplayMode m_informationDisplayMode;

    void  updateInformationTab(CallgrindCallstackItem *item);
    void  updateCallerTab(CallgrindCallstackItem *item);
    void  updateCalleeTab(CallgrindCallstackItem *item);
};
}
#endif // CALLGRINDVIEW_H
