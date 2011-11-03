/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>

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

#include "massifparser.h"

namespace valgrind
{

    MassifParser::MassifParser(QObject *parent)
    {
	Q_UNUSED(parent);
    }

    MassifParser::~MassifParser()
    {
    }

    void MassifParser::parse()
    {
	while (!device()->atEnd())
	{
	    m_buffer = device()->readLine();
	    if (m_buffer.startsWith("#") || m_buffer.startsWith("desc")
	        || m_buffer.startsWith("time_unit"))
		continue; // skip comment and useless lines
	    if (m_buffer.startsWith("cmd"))
	    {
		m_workingDir = m_buffer.split(' ')[1].split("build")[0];
		continue;
	    }
	    m_lst = m_buffer.split("=");
	    if (m_lst[0] == "snapshot") // new snapshot
	      {
		m_item = new MassifItem();
		m_item->incomingData(m_lst[0], m_lst[1].trimmed());
		continue;
	      }
	    m_item->incomingData(m_lst[0], m_lst[1].trimmed());
	    if (m_lst[0] == "heap_tree")
	    {
	      if (m_lst[1].startsWith("peak") || m_lst[1].startsWith("detailed"))
		while (!m_buffer.startsWith("#") && !device()->atEnd())
		  {
		    m_buffer = device()->readLine();
		    if (m_buffer.startsWith("#"))
		      break;
		    MassifItem *child = new MassifItem(true);
		    child->setParent(m_item);
		    child->incomingData("child", m_buffer.trimmed(), m_workingDir);
		    m_item->appendChild(child);
		  }
	      // this is the last info, send the item to the model
	      emit newItem(m_item);
	    }
	}
	emit newItem(NULL);
    }
}
#include "massifparser.moc"
