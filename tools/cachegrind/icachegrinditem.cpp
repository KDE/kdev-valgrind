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

#include "icachegrinditem.h"
namespace valgrind
{
QString iCachegrindItem::dataKey(int column)
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

iCachegrindItem::Columns  iCachegrindItem::dataKeyFromName(const QString& keyName)
{
    if (keyName == "Ir")
        return iCachegrindItem::InstructionRead;
    else if (keyName == "I1mr")
        return iCachegrindItem::InstructionL1ReadMiss;
    else if (keyName == "ILmr")
        return iCachegrindItem::InstructionLLReadMiss;
    else if (keyName == "Dr")
        return iCachegrindItem::DataCacheRead;
    else if (keyName == "D1mr")
        return iCachegrindItem::DataCacheD1ReadMiss;
    else if (keyName == "DLmr")
        return iCachegrindItem::DataCacheLLReadMiss;
    else if (keyName == "Dw")
        return iCachegrindItem::DataCacheWrite;
    else if (keyName == "D1mw")
        return iCachegrindItem::DataCacheD1WriteMiss;
    else if (keyName == "DLmw")
        return iCachegrindItem::DataCacheLLWriteMiss;
    else if (keyName == "Bc")
        return iCachegrindItem::ConditionnalBranchExecute;
    else if (keyName == "Bcm")
        return iCachegrindItem::ConditionnalBranchMisprediced;
    else if (keyName == "Bi")
        return iCachegrindItem::IndirectBranchExecuted;
    else if (keyName == "Bim")
        return iCachegrindItem::IndirectBranchMispredicted;
    else
        return iCachegrindItem::Unknow;
}

bool iCachegrindItem::isNumericValue(int column)
{
    if (column == CallName || column == FileName || column == Unknow)
        return false;
    return true;
}
}
