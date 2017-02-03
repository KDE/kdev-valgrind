/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#pragma once

#include <QString>
#include <QVariant>
#include <QMap>

#define CALLGRIND_MIN_DISPLAY_PERCENT 1 // FIXME remove ?

namespace valgrind
{

class CallgrindItem
{
public:
    /**
     * Be carefull ! the order is important
     * if you want to add an enum please do it by the end
     */
    enum Columns
    {
        CallName = 0,
        FileName,
        InstructionRead,//Ir
        InstructionL1ReadMiss,//I1mr
        InstructionLLReadMiss,//ILmr
        DataCacheRead,//Dr
        DataCacheD1ReadMiss,//D1mr
        DataCacheLLReadMiss,//DLmr
        DataCacheWrite,//Dw
        DataCacheD1WriteMiss,//D1mw
        DataCacheLLWriteMiss,//DLmw
        ConditionnalBranchExecute,//Bc
        ConditionnalBranchMisprediced,//Bcm
        IndirectBranchExecuted,//Bi
        IndirectBranchMispredicted,//Bim
        NumberOfCalls,

        Unknow
    };

    static QString  dataKey(int column);
    static Columns  dataKeyFromName(const QString& keyName);
    static bool     isNumericValue(int column);
};

class CallgrindCallstackFunction;

class CallgrindCallstackItem
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

    void addChild(CallgrindCallstackItem* child);
    CallgrindCallstackItem* child(int n) const;
    int childCount() const;

    void addParent(CallgrindCallstackItem* parent);
    CallgrindCallstackItem* parent(int n) const;
    int parentCount() const;

    void setNumericValue(int n, int val);
    int numericValue(int n) const;
    unsigned long long numericValue(CallgrindItem::Columns col) const;

    void setValue(const QString& key, const QString& value);

    bool hasKey(int n);

    unsigned long long includeNumericValue(CallgrindItem::Columns col) const;
    double includeNumericValuePercent(CallgrindItem::Columns col) const;
    double numericValuePercent(CallgrindItem::Columns col) const;

    QVariant data(CallgrindItem::Columns col, numberDisplayMode disp = E_NORMAL) const;

    CallgrindCallstackFunction* csFunction() const;

    void setNumCalls(int calls) { m_numCalls = calls; }
    int numCalls() const { return m_numCalls; }

private:
    // The number of calls for this function
    int m_numCalls;
    QMap<int, int> m_numericValue;

    CallgrindCallstackItem* totalCountItem() const;

    QVariant numericValue(CallgrindItem::Columns col, numberDisplayMode disp) const;

    CallgrindCallstackFunction* m_csFunction;

    QList<CallgrindCallstackItem*> m_parents;
    QList<CallgrindCallstackItem*> m_childs;
};

class CallgrindCallstackFunction
{
public:
    void setFilename(const QString& fn);
    const QString& filename() const;

    void setFunctionName(const QString& fn);
    const QString& functionName() const;

    void setFullDescName(const QString& fdn);
    const QString& fullDescName() const;

    void setTotalCountItem(CallgrindCallstackItem* totalCountItem);
    CallgrindCallstackItem* totalCountItem() const;

private:
    // format : file:function[binary]
    QString m_fullDescName;
    QString m_fileName;
    QString m_fctName;

    CallgrindCallstackItem* m_totalCountItem;
};

}
