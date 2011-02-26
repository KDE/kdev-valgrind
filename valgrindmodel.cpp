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

#include "valgrinditem.h"
#include "valgrindmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>


ValgrindModel::~ ValgrindModel( )
{
    qDeleteAll(errors);
}

int ValgrindModel::columnCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent)
    return numColumns;
}

QVariant ValgrindModel::data ( const QModelIndex & index, int role ) const
{
    ValgrindItem* item = itemForIndex(index);

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                /*case Index:
                    if (ValgrindError* e = dynamic_cast<ValgrindError*>(item))
                        return e->uniqueId;
                    else if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
                        return f->line;
                    break;*/

                case Function:
                    if (ValgrindError* e = dynamic_cast<ValgrindError*>(item))
                        return e->what;
                    else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(item))
                        return s->what();
                    else if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item)) {
                        QString ret;
                        if (!f->fn.isEmpty())
                            ret = f->fn;
                        else
                            ret = QString("0x%1").arg(QString::number(f->instructionPointer, 16));
                        if (f == f->parent()->frames.first())
                            return i18n("at: %1", ret);
                        else
                            return i18n("by: %1", ret);
                    }
                    break;

                case Source:
                    if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
                        if (!f->file.isEmpty()) {
                            if (f->line >= 0)
                                return f->file + ':' + f->line;
                            else
                                return f->file;
                        }
                    break;

                case Object:
                    if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
                        return f->obj;
                    break;
            }
            break;

        case Qt::FontRole:
            switch (index.column()) {
                case Function:
                    if (dynamic_cast<ValgrindFrame*>(item)) {
                        return KGlobalSettings::fixedFont();
                    } else if (dynamic_cast<ValgrindError*>(item)) {
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
                    if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item)) {
                        if (f->line >= 0)
                            return f->url().toLocalFile() + ':' + f->line;
                        else
                            return f->url().toLocalFile();
                    }
                    break;
            }
            break;
    }

    return QVariant();
}

QVariant ValgrindModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QModelIndex ValgrindModel::index ( int row, int column, const QModelIndex & p ) const
{
    if (row < 0 || column < 0 || column >= numColumns)
        return QModelIndex();

    if (p.isValid() && p.column() != 0)
        return QModelIndex();

    ValgrindItem* parent = itemForIndex(p);

    if (!parent) {
        if (row < errors.count())
            return createIndex(row, column, errors.at(row));

    } else if (ValgrindError* e = dynamic_cast<ValgrindError*>(parent)) {
        int r2 = row;

        if (e->stack) {
            if (row < e->stack->frames.count())
                return createIndex(row, column, e->stack->frames.at(row));

            r2 -= e->stack->frames.count();
        }

        if (r2 == 0 && e->auxStack)
            return createIndex(row, column, e->auxStack);

    } else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(parent)) {
        if (row < s->frames.count())
            return createIndex(row, column, s->frames[row]);
    }

    return QModelIndex();
}

QModelIndex ValgrindModel::parent ( const QModelIndex & index ) const
{
    ValgrindItem* item = itemForIndex(index);
    if (!item)
        return QModelIndex();

    item = item->parent();

    /*if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(s))
        if (s == s->parent()->stack)
            item = item->parent();*/

    return indexForItem(item, 0);
}

int ValgrindModel::rowCount ( const QModelIndex & p ) const
{
    if (!p.isValid())
        return errors.count();

    if (p.column() != 0)
        return 0;

    ValgrindItem* parent = itemForIndex(p);

    if (ValgrindError* e = dynamic_cast<ValgrindError*>(parent)) {
        int ret = 0;
        if (e->stack)
            ret += e->stack->frames.count();

        if (e->auxStack)
            ++ret;

        return ret;
    }

    else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(parent))
        return s->frames.count();

    return 0;
}

QModelIndex ValgrindModel::indexForItem( ValgrindItem* item, int column ) const
{
    int index = -1;

    if (ValgrindError* e = dynamic_cast<ValgrindError*>(item))
        index = e->parent()->errors.indexOf(e);
    else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(item))
        if (s == s->parent()->stack)
            return indexForItem(s->parent());
        else if (s->parent()->stack)
            index = s->parent()->stack->frames.count();
        else
            index = 0;
    else if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
        index = f->parent()->frames.indexOf(f);

    if (index != -1)
        return createIndex(index, column, item);

    return QModelIndex();
}

ValgrindItem* ValgrindModel::itemForIndex(const QModelIndex& index) const
{
    if (index.internalPointer())
        return static_cast<ValgrindItem*>(index.internalPointer());

    return 0L;
}

ValgrindModel::ValgrindModel(QObject * parent)
    : QAbstractItemModel(parent)
{
    m_error = 0L;
    m_stack = 0L;
    m_frame = 0L;
}

void ValgrindModel::newElement(ValgrindModel::eElementType e)
{
    qDebug() << "New Element" << e;
    switch (e) {
    case startError:
	newStartError();
	break;
    case error:
	newError();
	break;
    case startStack:
	m_stack = new ValgrindStack(this, m_error);
	break;
    case startFrame:
	m_frame = new ValgrindFrame(m_stack);
	break;
    default:
	break;
    }
}

void ValgrindModel::newData(ValgrindModel::eElementType e, QString name, QString value)
{
    qDebug() << "New Data" << e << " " << name << " " << value;
    switch (e) {
    case error:
	m_error->incomingData(name, value);
	break;
    case frame:
	m_frame->incomingData(name, value);
	break;
    case stack:
	m_stack->incomingData(name, value);
	break;
    default:
	incomingData(name, value);
    }
}

void ValgrindModel::newStartError()
{
    m_error = new ValgrindError(this);
    beginInsertRows(QModelIndex(), errors.count(), errors.count());
    errors.append(m_error);
    endInsertRows();
}

void ValgrindModel::newError()
{
    beginInsertRows(indexForItem(m_error), m_error->stack->frames.count(), m_error->stack->frames.count());
    m_error->auxStack = m_stack;
    endInsertRows();
}

void ValgrindModel::newFrame()
{
    if (m_stack == m_stack->parent()->stack)
	beginInsertRows(indexForItem(m_error), m_stack->frames.count(), m_stack->frames.count());
    m_stack->frames.append(m_frame);
    if (m_stack == m_stack->parent()->stack)
	endInsertRows();
    m_frame = 0L;
}

void ValgrindModel::reset()
{
    qDeleteAll(errors);
    errors.clear();
    reset();
}

void ValgrindModel::incomingData(QString name, QString value)
{
// case Preamble:
//     if (name() == "line")
// 	preamble.append(m_buffer);
//     break;


  //case Root:
  // if (name() == "m_protocolVersion")
  //     m_protocolVersion = m_buffer.toInt();
  // else if (name() == "pid")
  //     pid = m_buffer.toInt();
  // else if (name() == "ppid")
  //     ppid = m_buffer.toInt();
  // else if (name() == "tool")
  //     tool = m_buffer;
  // else if (name() == "usercomment")
  //     userComment = m_buffer;
  // //          else if (name() == "error")
  // //  m_currentError = 0L;
  // break;

}

void ValgrindModel::insertIntoTree(ValgrindModel::eElementType e)
{
  if (e == error)
    {
      beginInsertRows(indexForItem(m_error), m_error->stack->frames.count(), m_error->stack->frames.count());
      m_error->auxStack = m_stack;
      endInsertRows();
    }
  else if (e == frame)
    {
      beginInsertRows(indexForItem(m_error), m_stack->frames.count(), m_stack->frames.count());
      if (m_stack == m_stack->parent()->stack)
  	endInsertRows();
   }
}

#include "valgrindmodel.moc"

