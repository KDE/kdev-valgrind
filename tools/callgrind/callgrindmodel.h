/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>

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

#ifndef _CALLGRINDMODEL_H_
#define _CALLGRINDMODEL_H_

#include "imodel.h"

#include <QList>

#include "cachegrind/icachegrinditem.h" // FIXME move iCachegrindItem to common interfaces ?
#include "callgrinditem.h"

namespace valgrind
{
class CallgrindCallstackItem;
class CallgrindFunctionsListTModel;

typedef bool (CallgrindFunctionsListTModel::*CSItemCompareFct)(const QVariant &, const QVariant &) const;

/**
 * This class will contains all the callgrind data models for the differents view
 */
class CallgrindModel :  public QObject,
                        public Model
{
    Q_OBJECT

public:
    explicit CallgrindModel(QObject* parent = nullptr);
    ~CallgrindModel() override;

    QAbstractItemModel  *getQAbstractItemModel(int n) override;

    const QList<CallgrindCallstackItem*>& getAllCsItem() const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    enum qAbstractItemModel
    {
        E_FCT_LIST,
        E_INFO_LIST,
        E_CALLER_LIST,
        E_CALLEE_LIST
    };

    /////SLOTS WRAPPER////
    /**
     * Reception of a new item in the model
     */
    void newItem(ModelItem *item) override;
    /**
     * Resets the model content
     */
    void reset() override;
    ////END SLOTS WRAPER////
private:
    QList<CallgrindCallstackItem*>  m_callgrindCsItems;

    CallgrindCallstackItem          *m_totalCountItem;
    CallgrindFunctionsListTModel    *m_callgrindFunctionModel;
};

/**
 * Main view model
 */
class CallgrindFunctionsListTModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CallgrindFunctionsListTModel(CallgrindModel* mdl);

    void                      setItemList(const QList<CallgrindCallstackItem *> items);

    //inherit methods from QAbstractItemModel implementation
    QModelIndex               index(int, int, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex               parent(const QModelIndex&) const override;
    int                       rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int                       columnCount(const QModelIndex&) const override;
    QVariant                  data(const QModelIndex &index, int role) const override;
    QVariant                  headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    CallgrindCallstackItem   *itemForIndex(const QModelIndex &index) const;
    void                      sort ( int column, Qt::SortOrder order) override;

    //Make the links between list comumn and data model
    iCachegrindItem::Columns  columnToPosInModelList(int col) const;
    CallgrindCallstackItem::numberDisplayMode columnToDisplayMode(int col) const;
private:
    //sort implementation
    void                      quickSortCSItem( int first, int last,
                                               QList<CallgrindCallstackItem*>& list,
                                               iCachegrindItem::Columns col,
                                               CSItemCompareFct cmp,
                                               CallgrindCallstackItem::numberDisplayMode dispMode );
    bool                      lessThan(const QVariant &left, const QVariant &right) const;
    bool                      greatherThan(const QVariant &left, const QVariant &right) const;

    /** list of all the items */
    QList<CallgrindCallstackItem *> m_items;
    CallgrindModel                  *m_model;
};

}
#endif /* _CALLGRINDMODEL_H_ */
