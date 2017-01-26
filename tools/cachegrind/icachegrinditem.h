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

#ifndef _ICACHEGRINDITEM_H__
#define _ICACHEGRINDITEM_H__

#include "interfaces/imodel.h"

namespace valgrind
{
//callgrind and cachegrind are simiral, they share the same item
class iCachegrindItem : public ModelItem
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
}

#endif
