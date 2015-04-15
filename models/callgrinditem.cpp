/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Sarie Lucas <lucas.sarie@gmail.com>

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

#include "callgrinditem.h"
#include <QDebug>
#include <kdebug.h>
#include "debug.h"


namespace valgrind
{

CallgrindCallstackItem::CallgrindCallstackItem(CallgrindCallstackFunction *function)
{
    Q_ASSERT(function);
    m_csFunction = function;
    m_numCalls = 0;
}

void  CallgrindCallstackItem::addChild(CallgrindCsItem *child)
{
    m_childs.append(child);
}

CallgrindCallstackItem  *CallgrindCallstackItem::getChild(int n) const
{
    return m_childs.at(n);
}

int CallgrindCallstackItem::getChildCount() const
{
    return m_childs.size();
}

void  CallgrindCallstackItem::addParent(CallgrindCsItem *child)
{
    m_parents.append(child);
}

CallgrindCallstackItem  *CallgrindCallstackItem::getParent(int n) const
{
    return m_parents.at(n);
}

int CallgrindCallstackItem::getParentCount() const
{
    return m_parents.size();
}

CallgrindCallstackFunction  *CallgrindCallstackItem::getCsFunction() const
{
    return m_csFunction;
}

CallgrindCallstackItem  *CallgrindCallstackItem::getTotalCountItem() const
{
    return (m_csFunction->getTotalCountItem());
}

bool CallgrindCallstackItem::hasKey(int n)
{
    if (n == 0 || n == 1 || n == 2 || m_numericValue.contains(n))
        return true;
    return false;
}

void  CallgrindCallstackItem::addNumericValue(int n, int val)
{
    m_numericValue[n] = val;
}

int   CallgrindCallstackItem::getNumericValue(int n) const
{
    return m_numericValue[n];
}

void  CallgrindCallstackItem::incommingData(const QString& key, const QString& value)
{
    int iKey = iCachegrindItem::dataKeyFromName(key);
    if (iKey == iCachegrindItem::Unknow)
        return;
    if (iCachegrindItem::isNumericValue(iKey))
    {
        m_numericValue[iKey] = value.toInt();
    }
    //else if (iKey == iCachegrindItem::FileName)
    //    m_fileName = value;
    //else
    //    m_fctName = value;
}

QVariant  CallgrindCallstackItem::data(iCachegrindItem::Columns key, numberDisplayMode disp) const
{
    switch (key)
    {
    case iCachegrindItem::FileName:
        return this->m_csFunction->getFilename();
    case iCachegrindItem::CallName:
        return this->m_csFunction->getFunctionName();
    case iCachegrindItem::NumberOfCalls:
        return m_numCalls;
    default:
        return (this->getNumericValue(key, disp));
    }
    return QVariant();
}

QVariant  CallgrindCallstackItem::getNumericValue(iCachegrindItem::Columns key, numberDisplayMode disp) const
{
    //TODO: REPLACE WITH PTR ON FCT
    switch (disp)
    {
    case E_NORMAL:
        return this->getNumericValue(key);
    case E_INCLUDE_NORMAL:
        return this->getIncludeNumericValue(key);
    case E_PERCENT:
        return this->getNumericValuePercent(key);
    case E_INCLUDE_PERCENT:
        return this->getIncludeNumericValuePercent(key);
    }
    return 0;
}

unsigned long long CallgrindCallstackItem::getNumericValue(iCachegrindItem::Columns col) const
{
    return (m_numericValue[col]);
}

unsigned long long CallgrindCallstackItem::getIncludeNumericValue(iCachegrindItem::Columns col) const
{
    unsigned long long includeValue = 0;
    for (int i = 0; i < m_parents.size(); ++i)
    {
        includeValue += m_parents[i]->getNumericValue(col);
    }
    return (includeValue);
}

double CallgrindCallstackItem::getNumericValuePercent(iCachegrindItem::Columns col) const
{
    return (double) ((int) (((float) getNumericValue(col)) / ((float) getTotalCountItem()->getNumericValue(col)) * 10000)) / 100;
}

double CallgrindCallstackItem::getIncludeNumericValuePercent(iCachegrindItem::Columns col) const
{
    unsigned long long includeValue = 0;
    for (int i = 0; i < m_parents.size(); ++i)
    {
        includeValue += m_parents[i]->getNumericValue(col);
    }
    return (double) ((int) (((float) includeValue) / ((float) getTotalCountItem()->getNumericValue(col)) * 10000)) / 100;
}

///////////////////////////////////////////////////////////////////////////////
//                         CallgrindCallstackFunction                        //
///////////////////////////////////////////////////////////////////////////////

void  CallgrindCallstackFunction::setTotalCountItem(CallgrindCallstackItem *totalCountItem)
{
    m_totalCountItem = totalCountItem;
}

CallgrindCallstackItem  *CallgrindCallstackFunction::getTotalCountItem() const
{
    return (m_totalCountItem);
}

void  CallgrindCallstackFunction::setFilename(const QString& fn)
{
    m_fileName = fn;
}

const QString& CallgrindCallstackFunction::getFilename() const
{
    return m_fileName;
}

void  CallgrindCallstackFunction::setFunctionName(const QString& fn)
{
    m_fctName = fn;
}

const QString& CallgrindCallstackFunction::getFunctionName() const
{
    return m_fctName;
}

void CallgrindCallstackFunction::setFullDescName(const QString& fdn)
{
    int iEnd, iBegin;

    iBegin = 0;
    m_fullDescName = fdn;
    //file name
    if ((iEnd = fdn.indexOf(QChar(':'), iBegin)) == -1)
        return;
    this->m_fileName = fdn.mid(iBegin, iEnd - iBegin);
    //function name name
    iBegin = iEnd + 1;
    iEnd = fdn.length();
    this->m_fctName = fdn.mid(iBegin, iEnd - iBegin);

}

const QString& CallgrindCallstackFunction::getFullDescName() const
{
    return m_fullDescName;
}
}
