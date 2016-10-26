/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
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

#ifndef _CALLGRINDITEM_H_
#define _CALLGRINDITEM_H_

#include <QString>
#include <QVariant>
#include <QMap>

#include "icachegrinditem.h"
#include "imodel.h"

#define CALLGRIND_MIN_DISPLAY_PERCENT 1

namespace valgrind
{

class CallgrindCallstackItem;
typedef CallgrindCallstackItem CallgrindCsItem;

class CallgrindCallstackFunction;

class CallgrindCallstackItem : public ModelItem
{
public:
    explicit CallgrindCallstackItem(CallgrindCallstackFunction* function);

    enum numberDisplayMode
    {
        E_NORMAL,
        E_INCLUDE_NORMAL,
        E_PERCENT,
        E_INCLUDE_PERCENT
    };

    void                      addChild(CallgrindCallstackItem *child);
    CallgrindCallstackItem    *getChild(int n) const;
    int                       getChildCount() const;

    void                      addParent(CallgrindCallstackItem *parent);
    CallgrindCallstackItem    *getParent(int n) const;
    int                       getParentCount() const;

    void                      addNumericValue(int n, int val);
    int                       getNumericValue(int n) const;
    void                      incommingData(const QString& key, const QString& value);
    bool                      hasKey(int n);
    void                      setTotalCountItem(CallgrindCallstackItem *totalCountItem);

    unsigned long long        getNumericValue(iCachegrindItem::Columns col) const;
    unsigned long long        getIncludeNumericValue(iCachegrindItem::Columns col) const;
    double                    getNumericValuePercent(iCachegrindItem::Columns col) const;
    double                    getIncludeNumericValuePercent(iCachegrindItem::Columns col) const;

    QVariant                  data(iCachegrindItem::Columns col, numberDisplayMode disp = E_NORMAL) const;
    CallgrindCallstackFunction *getCsFunction() const;
    //const QString&            getFullDescName() const { return m_fullDescName; }
    int                         getNumCalls() const { return m_numCalls; }
    void                        setNumCalls(int calls) { m_numCalls = calls; }

private:
    // The number of calls for this function
    int m_numCalls;
    QMap<int, int>             m_numericValue;

    CallgrindCallstackItem    *getTotalCountItem() const;

    QVariant                   getNumericValue(iCachegrindItem::Columns col, numberDisplayMode disp) const;

    CallgrindCallstackFunction *m_csFunction;

    QList<CallgrindCallstackItem *>   m_parents;
    QList<CallgrindCallstackItem *>   m_childs;
};

class CallgrindCallstackFunction
{
public:
    void                      setFilename(const QString& fn);
    const QString&            getFilename() const;
    void                      setFunctionName(const QString& fn);
    const QString&            getFunctionName() const;
    void                      setFullDescName(const QString& fdn);
    const QString&            getFullDescName() const;

    void                      setTotalCountItem(CallgrindCallstackItem *totalCountItem);
    CallgrindCallstackItem    *getTotalCountItem() const;

private:
    //format : file:function[binary]
    QString                           m_fullDescName;
    QString                           m_fileName;
    QString                           m_fctName;

    CallgrindCallstackItem*           m_totalCountItem;

};
}

#endif /* _CALLGRINDITEM_H_ */
