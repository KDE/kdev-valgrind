/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Sarie Lucas <lucas.sarie@gmail.com>
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

#include "modelitem.h"

#include "debug.h"

namespace valgrind
{

QString CallgrindItem::dataKey(int column)
{
    switch (column)
    {
    case CallName:
        return QString("cn");
    case FileName:
        return QString("fn");
    case InstructionRead:
        return QString("Ir");
    case InstructionL1ReadMiss:
        return QString("I1mr");
    case InstructionLLReadMiss:
        return QString("ILmr");
    case DataCacheRead:
        return QString("Dr");
    case DataCacheD1ReadMiss:
        return QString("D1mr");
    case DataCacheLLReadMiss:
        return QString("DLmr");
    case DataCacheWrite:
        return QString("Dw");
    case DataCacheD1WriteMiss:
        return QString("D1mw");
    case DataCacheLLWriteMiss:
        return QString("DLmw");
    case ConditionnalBranchExecute:
        return QString("Bc");
    case ConditionnalBranchMisprediced:
        return QString("Bcm");
    case IndirectBranchExecuted:
        return QString("Bi");
    case IndirectBranchMispredicted:
        return QString("Bim");
    case Unknow:
        return QString("???");
    }
    return QString();
}

CallgrindItem::Columns  CallgrindItem::dataKeyFromName(const QString& keyName)
{
    if (keyName == "Ir") {
        return CallgrindItem::InstructionRead;
    }

    else if (keyName == "I1mr") {
        return CallgrindItem::InstructionL1ReadMiss;
    }

    else if (keyName == "ILmr") {
        return CallgrindItem::InstructionLLReadMiss;
    }

    else if (keyName == "Dr") {
        return CallgrindItem::DataCacheRead;
    }

    else if (keyName == "D1mr") {
        return CallgrindItem::DataCacheD1ReadMiss;
    }

    else if (keyName == "DLmr") {
        return CallgrindItem::DataCacheLLReadMiss;
    }

    else if (keyName == "Dw") {
        return CallgrindItem::DataCacheWrite;
    }

    else if (keyName == "D1mw") {
        return CallgrindItem::DataCacheD1WriteMiss;
    }

    else if (keyName == "DLmw") {
        return CallgrindItem::DataCacheLLWriteMiss;
    }

    else if (keyName == "Bc") {
        return CallgrindItem::ConditionnalBranchExecute;
    }

    else if (keyName == "Bcm") {
        return CallgrindItem::ConditionnalBranchMisprediced;
    }

    else if (keyName == "Bi") {
        return CallgrindItem::IndirectBranchExecuted;
    }

    else if (keyName == "Bim") {
        return CallgrindItem::IndirectBranchMispredicted;
    }

    return CallgrindItem::Unknow;
}

bool CallgrindItem::isNumericValue(int column)
{
    if (column == CallName || column == FileName || column == Unknow) {
        return false;
    }

    return true;
}

CallgrindCallstackItem::CallgrindCallstackItem(CallgrindCallstackFunction* function)
    : m_numCalls(0)
    , m_csFunction(function)
{
    Q_ASSERT(function);
}

void CallgrindCallstackItem::addChild(CallgrindCallstackItem* child)
{
    m_childs.append(child);
}

CallgrindCallstackItem* CallgrindCallstackItem::child(int n) const
{
    return m_childs.at(n);
}

int CallgrindCallstackItem::childCount() const
{
    return m_childs.size();
}

void  CallgrindCallstackItem::addParent(CallgrindCallstackItem* child)
{
    m_parents.append(child);
}

CallgrindCallstackItem* CallgrindCallstackItem::parent(int n) const
{
    return m_parents.at(n);
}

int CallgrindCallstackItem::parentCount() const
{
    return m_parents.size();
}

CallgrindCallstackFunction* CallgrindCallstackItem::csFunction() const
{
    return m_csFunction;
}

CallgrindCallstackItem* CallgrindCallstackItem::totalCountItem() const
{
    return m_csFunction->totalCountItem();
}

bool CallgrindCallstackItem::hasKey(int n)
{
    if (n == 0 || n == 1 || n == 2 || m_numericValue.contains(n)) {
        return true;
    }

    return false;
}

void CallgrindCallstackItem::setNumericValue(int n, int val)
{
    m_numericValue[n] = val;
}

int CallgrindCallstackItem::numericValue(int n) const
{
    return m_numericValue[n];
}

void CallgrindCallstackItem::setValue(const QString& key, const QString& value)
{
    int iKey = CallgrindItem::dataKeyFromName(key);
    if (iKey == CallgrindItem::Unknow) {
        return;
    }

    if (CallgrindItem::isNumericValue(iKey)) {
        m_numericValue[iKey] = value.toInt();
    }

    //else if (iKey == CallgrindItem::FileName)
    //    m_fileName = value;
    //else
    //    m_fctName = value;
}

QVariant CallgrindCallstackItem::data(CallgrindItem::Columns key, numberDisplayMode disp) const
{
    switch (key) {

    case CallgrindItem::FileName:
        return m_csFunction->filename();

    case CallgrindItem::CallName:
        return m_csFunction->functionName();

    case CallgrindItem::NumberOfCalls:
        return m_numCalls;

    default:
        return (this->numericValue(key, disp));

    }

    return QVariant();
}

QVariant CallgrindCallstackItem::numericValue(CallgrindItem::Columns key, numberDisplayMode disp) const
{
    // TODO: REPLACE WITH PTR ON FCT
    switch (disp) {

    case E_NORMAL:
        return numericValue(key);

    case E_INCLUDE_NORMAL:
        return includeNumericValue(key);

    case E_PERCENT:
        return numericValuePercent(key);

    case E_INCLUDE_PERCENT:
        return includeNumericValuePercent(key);

    }

    return 0;
}

unsigned long long CallgrindCallstackItem::numericValue(CallgrindItem::Columns col) const
{
    return (m_numericValue[col]);
}

unsigned long long CallgrindCallstackItem::includeNumericValue(CallgrindItem::Columns col) const
{
    unsigned long long includeValue = 0;
    for (int i = 0; i < m_parents.size(); ++i)
    {
        includeValue += m_parents[i]->numericValue(col);
    }

    return includeValue;
}

double CallgrindCallstackItem::numericValuePercent(CallgrindItem::Columns col) const
{
    return (double) ((int) (((float) numericValue(col)) / ((float) totalCountItem()->numericValue(col)) * 10000)) / 100;
}

double CallgrindCallstackItem::includeNumericValuePercent(CallgrindItem::Columns col) const
{
    unsigned long long includeValue = 0;
    for (int i = 0; i < m_parents.size(); ++i) {
        includeValue += m_parents[i]->numericValue(col);
    }

    return (double) ((int) (((float) includeValue) / ((float) totalCountItem()->numericValue(col)) * 10000)) / 100;
}

void CallgrindCallstackFunction::setTotalCountItem(CallgrindCallstackItem* totalCountItem)
{
    m_totalCountItem = totalCountItem;
}

CallgrindCallstackItem* CallgrindCallstackFunction::totalCountItem() const
{
    return m_totalCountItem;
}

void CallgrindCallstackFunction::setFilename(const QString& fn)
{
    m_fileName = fn;
}

const QString& CallgrindCallstackFunction::filename() const
{
    return m_fileName;
}

void CallgrindCallstackFunction::setFunctionName(const QString& fn)
{
    m_fctName = fn;
}

const QString& CallgrindCallstackFunction::functionName() const
{
    return m_fctName;
}

void CallgrindCallstackFunction::setFullDescName(const QString& fdn)
{
    int iBegin;
    int iEnd;

    iBegin = 0;
    m_fullDescName = fdn;

    // file name
    if ((iEnd = fdn.indexOf(QChar(':'), iBegin)) == -1) {
        return;
    }

    m_fileName = fdn.mid(iBegin, iEnd - iBegin);

    // function name
    iBegin = iEnd + 1;
    iEnd = fdn.length();
    m_fctName = fdn.mid(iBegin, iEnd - iBegin);
}

const QString& CallgrindCallstackFunction::fullDescName() const
{
    return m_fullDescName;
}

}
