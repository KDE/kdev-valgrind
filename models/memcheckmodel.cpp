/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#include "memcheckitemsimpl.h"
#include "memcheckmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>

#include <iostream>

namespace valgrind
{

  MemcheckModel::MemcheckModel(QObject * parent)
  {
  }

  MemcheckModel::~ MemcheckModel()
  {
    //    qDeleteAll(errors);
  }

  int MemcheckModel::columnCount ( const QModelIndex & parent ) const
  {
    Q_UNUSED(parent)
      return numColumns;
  }

  QVariant MemcheckModel::data ( const QModelIndex & index, int role ) const
  {
    MemcheckItem* item = itemForIndex(index);

    switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
	/*case Index:
	  if (MemcheckError* e = dynamic_cast<MemcheckError*>(item))
	  return e->uniqueId;
	  else if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item))
	  return f->line;
	  break;*/

      case Function:
	if (MemcheckError* e = dynamic_cast<MemcheckError*>(item)) {
	  if (e->what.isEmpty())
	    return e->text;
	  else
	    return e->what;
	}
	else if (MemcheckStack* s = dynamic_cast<MemcheckStack*>(item))
	  return s->what();
	else if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item)) {
	  QString ip;
	  QString func;
	  ip = QString("0x%1").arg(QString::number(f->instructionPointer, 16));
	  if (!f->fn.isEmpty())
	    func = f->fn;
	  if (!f->file.isEmpty()) {
	    func += " (";
	    func += f->file;
	    func += " ";
	    func += QString("%1").arg(QString::number(f->line));
	    func += ")";
	  }
	  if (f == f->parent()->getFrames().front())
	    return i18n("at %1: %2", ip, func);
	  else
	    return i18n("by %1: %2", ip, func);
	}
	break;

      case Source:
	if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item))
	  if (!f->file.isEmpty()) {
	    if (f->line >= 0)
	      return f->file + ':' + f->line;
	    else
	      return f->file;
	  }
	break;

      case Object:
	if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item))
	  return f->obj;
	break;
      }
      break;

    case Qt::FontRole:
      switch (index.column()) {
      case Function:
	if (dynamic_cast<MemcheckFrame*>(item)) {
	  return KGlobalSettings::fixedFont();
	} else if (dynamic_cast<MemcheckError*>(item)) {
	  QFont f = KGlobalSettings::generalFont();
	  f.setBold(true);
	  return f;
	}
	break;
      }
      break;

    case Qt::ToolTipRole:
      switch (index.column()) {
      case Source:
	if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item)) {
	  if (f->line >= 0)
	    return f->url().toLocalFile() + ':' + f->line;
	  else
	    return f->url().toLocalFile();
	}
	break;
      }
      break;

    case Qt::UserRole:
      switch (index.column()) {
      case Function:
	if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item))
	  if (!f->file.isEmpty())
	    return i18n("%1:%2", f->file, QString("%1").arg(QString::number(f->line)));
      }
      break;

    }

    return QVariant();
  }

  QVariant MemcheckModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    Q_UNUSED(orientation)
      switch (role) {
      case Qt::DisplayRole:
	switch (section) {
	  //case Index:
	  //return i18n("Item");
	case Function:
	  return i18n("Function");
	case Source:
	  return i18n("Source");
	case Object:
	  return i18n("Object");
        }
        break;
      }
    return QVariant();
  }

  QModelIndex MemcheckModel::index ( int row, int column, const QModelIndex & p ) const
  {
    if (row < 0 || column < 0 || column >= numColumns)
      return QModelIndex();

    if (p.isValid() && p.column() != 0)
      return QModelIndex();

    MemcheckItem* parent = itemForIndex(p);

    if (!parent) {
      if (row < m_errors.count())
	return createIndex(row, column, m_errors.at(row));

    } else if (MemcheckError* e = dynamic_cast<MemcheckError*>(parent)) {
      int r2 = row;

      foreach (MemcheckStack *stack, e->getStack())
	{
	  if (row < stack->getFrames().size())
	    return createIndex(row, column, stack->getFrames().at(row));
	  r2 -= stack->getFrames().count();
	}
    }
    // } else if (MemcheckStack* s = dynamic_cast<MemcheckStack*>(parent)) {
    //     if (row < s->frames.count())
    //         return createIndex(row, column, s->frames[row]);
    // }
    return QModelIndex();
  }

  QModelIndex MemcheckModel::parent ( const QModelIndex & index ) const
  {
    MemcheckItem* item = itemForIndex(index);
    if (!item)
      return QModelIndex();
    item = item->parent();
    return indexForItem(item, 0);
  }

  int MemcheckModel::rowCount ( const QModelIndex & p ) const
  {
    if (!p.isValid())
      return m_errors.count();

    if (p.column() != 0)
      return 0;

    MemcheckItem* parent = itemForIndex(p);

    if (MemcheckError* e = dynamic_cast<MemcheckError*>(parent)) {
      int ret = 0;
      foreach (const MemcheckStack *stack, e->getStack())
	ret += stack->getFrames().count();
      return ret;
    }
    return 0;
  }


  QModelIndex MemcheckModel::indexForItem(MemcheckItem* item, int column ) const
  {
    int index = -1;

    if (MemcheckError* e = dynamic_cast<MemcheckError*>(item))
      index = e->parent()->m_errors.indexOf(e);
    else if (MemcheckStack* s = dynamic_cast<MemcheckStack*>(item))
      if (s == s->parent()->lastStack())
	return indexForItem(s->parent());
      else if (s->parent()->getStack().count())
	index = s->parent()->lastStack()->getFrames().count();
      else
	index = 0;
    else if (MemcheckFrame* f = dynamic_cast<MemcheckFrame*>(item))
      index = f->parent()->getFrames().indexOf(f);
    if (index != -1)
      return createIndex(index, column, item);
    return QModelIndex();
  }

  MemcheckItem* MemcheckModel::itemForIndex(const QModelIndex& index) const
  {
    if (index.internalPointer())
      return static_cast<MemcheckItem*>(index.internalPointer());
    return 0L;
  }

  void MemcheckModel::newElement(MemcheckModel::eElementType e)
  {
  switch (e) {
    case startError:
      newStartError();
      break;
    case startStack:
      newStack();
      break;
    case startFrame:
      newFrame();
      break;
    default:
      break;
    }
  }

  void MemcheckModel::newStack()
  {
    if (m_errors.back()->getStack().count()) {
      beginInsertRows(indexForItem(m_errors.back()), m_errors.back()->getStack().count(),
		      m_errors.back()->getStack().count());
      endInsertRows();
    }
    m_errors.back()->addStack();
  }

  void MemcheckModel::newStartError()
  {
    m_errors << new MemcheckError(this);
  }


  void MemcheckModel::newFrame()
  {
    m_errors.back()->lastStack()->addFrame();
  }

  void MemcheckModel::reset()
  {
    //    qDeleteAll(errors);
    m_errors.clear();
    reset();
  }

  void MemcheckModel::newData(MemcheckModel::eElementType e, QString name, QString value)
  {
  switch (e) {
    case error:
      m_errors.back()->incomingData(name, value);
      break;
    case frame:
      m_errors.back()->lastStack()->lastFrame()->incomingData(name, value);
      break;
    case stack:
      m_errors.back()->lastStack()->incomingData(name, value);
      break;
    default:
      break;
    }
    emit modelChanged();
  }

  void MemcheckModel::incomingData(QString, QString)
  {
  }

}

#include "memcheckmodel.moc"

