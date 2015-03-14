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

#include "cachegrinditem.h"
#include <QDebug>
#include <QDir>

namespace valgrind
{

CachegrindItem::CachegrindItem()
{
}

CachegrindItem::~CachegrindItem()
{
    qDeleteAll(m_childItems);
}

QString CachegrindItem::dataKey(int column)
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

CachegrindItem::Columns  CachegrindItem::dataKeyFromName(const QString& keyName)
{
    if (keyName == "Ir")
        return CachegrindItem::InstructionRead;
    else if (keyName == "I1mr")
        return CachegrindItem::InstructionL1ReadMiss;
    else if (keyName == "ILmr")
        return CachegrindItem::InstructionLLReadMiss;
    else if (keyName == "Dr")
        return CachegrindItem::DataCacheRead;
    else if (keyName == "D1mr")
        return CachegrindItem::DataCacheD1ReadMiss;
    else if (keyName == "DLmr")
        return CachegrindItem::DataCacheLLReadMiss;
    else if (keyName == "Dw")
        return CachegrindItem::DataCacheWrite;
    else if (keyName == "D1mw")
        return CachegrindItem::DataCacheD1WriteMiss;
    else if (keyName == "DLmw")
        return CachegrindItem::DataCacheLLWriteMiss;
    else if (keyName == "Bc")
        return CachegrindItem::ConditionnalBranchExecute;
    else if (keyName == "Bcm")
        return CachegrindItem::ConditionnalBranchMisprediced;
    else if (keyName == "Bi")
        return CachegrindItem::IndirectBranchExecuted;
    else if (keyName == "Bim")
        return CachegrindItem::IndirectBranchMispredicted;
    else
        return CachegrindItem::Unknow;
}

bool CachegrindItem::isNumericValue(int column)
{
    if (column == CallName || column == FileName || column == Unknow)
        return false;
    return true;
}


void CachegrindItem::incomingData(const QString &name, const QVariant &value)
{
    m_values[name] = value;
}

void CachegrindItem::appendChild(CachegrindItem *item)
{
    m_childItems.append(item);
}

void CachegrindItem::setParent(CachegrindItem *parent)
{
    m_parentItem = parent;
}

CachegrindItem *CachegrindItem::child(int row)
{
    return m_childItems.value(row);
}

int CachegrindItem::childCount() const
{
    return m_childItems.count();
}

int CachegrindItem::columnCount() const
{
    return m_values.size();
}

int CachegrindItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<CachegrindItem*>(this));
    return 0;
}

CachegrindItem *CachegrindItem::parent() const
{
    return m_parentItem;
}

QVariant CachegrindItem::data(int column) const
{
    bool numeric = CachegrindItem::isNumericValue(column);
    QString key = CachegrindItem::dataKey(column);
    if (key.size() > 0) {
        if (numeric == true)
        {
          if (this->parent() == 0)
            return 0;
          if (this->parent()->parent() == 0)
            return 100;
          bool ok;
          float pc = (((float)m_values[key].toInt(&ok)) / ((float) this->parent()->m_values[key].toInt(&ok)));

          return ((float)((int)(pc * 10000))/100);
        }
        return m_values[key];
    }
    return QVariant();
}

QUrl CachegrindItem::url() const
{
    QString dir = m_values[CachegrindItem::dataKey(CachegrindItem::FileName)].toString();
    QString fName = m_values[CachegrindItem::dataKey(CachegrindItem::CallName)].toString();
    if (dir.isEmpty() && fName.isEmpty())
        return QUrl();

    QUrl url = QUrl::fromLocalFile(dir);

    return url;
}

}
