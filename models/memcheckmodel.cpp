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
#include "memcheckmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>


ValgrindModel::ValgrindModel(QObject * parent)
{
}

ValgrindModel::~ ValgrindModel()
{
  //    qDeleteAll(errors);
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
                    if (ValgrindError* e = dynamic_cast<ValgrindError*>(item)) {
			if (e->what.isEmpty())
			    return e->text;
			else
			    return e->what;
		    }
                    else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(item))
                        return s->what();
                    else if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item)) {
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

        case Qt::UserRole:
            switch (index.column()) {
                case Function:
		    if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
			if (!f->file.isEmpty())
			    return i18n("%1:%2", f->file, QString("%1").arg(QString::number(f->line)));
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
        if (row < m_errors.count())
            return createIndex(row, column, m_errors.at(row));

    } else if (ValgrindError* e = dynamic_cast<ValgrindError*>(parent)) {
        int r2 = row;

	foreach (ValgrindStack *stack, e->getStack())
	{
	    if (row < stack->getFrames().size())
		return createIndex(row, column, stack->getFrames().at(row));
	    r2 -= stack->getFrames().count();
	}
    }
    // } else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(parent)) {
    //     if (row < s->frames.count())
    //         return createIndex(row, column, s->frames[row]);
    // }
    return QModelIndex();
}

QModelIndex ValgrindModel::parent ( const QModelIndex & index ) const
{
    ValgrindItem* item = itemForIndex(index);
    if (!item)
        return QModelIndex();
    item = item->parent();
    return indexForItem(item, 0);
}

int ValgrindModel::rowCount ( const QModelIndex & p ) const
{
    if (!p.isValid())
        return m_errors.count();

    if (p.column() != 0)
        return 0;

    ValgrindItem* parent = itemForIndex(p);

    if (ValgrindError* e = dynamic_cast<ValgrindError*>(parent)) {
        int ret = 0;
	foreach (const ValgrindStack *stack, e->getStack())
	    ret += stack->getFrames().count();
        return ret;
    }
    return 0;
}

QModelIndex ValgrindModel::indexForItem( ValgrindItem* item, int column ) const
{
    int index = -1;

    if (ValgrindError* e = dynamic_cast<ValgrindError*>(item))
        index = e->parent()->m_errors.indexOf(e);
    else if (ValgrindStack* s = dynamic_cast<ValgrindStack*>(item))
        if (s == s->parent()->lastStack())
            return indexForItem(s->parent());
	else if (s->parent()->getStack().count())
	    index = s->parent()->lastStack()->getFrames().count();
        else
            index = 0;
    else if (ValgrindFrame* f = dynamic_cast<ValgrindFrame*>(item))
        index = f->parent()->getFrames().indexOf(f);
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

void ValgrindModel::newElementImple(ValgrindModel::eElementType e)
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

void ValgrindModel::newStack()
{
    if (m_errors.back()->getStack().count()) {
	beginInsertRows(indexForItem(m_errors.back()), m_errors.back()->getStack().count(),
			m_errors.back()->getStack().count());
	endInsertRows();
    }
    m_errors.back()->addStack();
}

void ValgrindModel::newStartError()
{
    m_errors << new ValgrindError(this);
}


void ValgrindModel::newFrame()
{
    m_errors.back()->lastStack()->addFrame();
}

void ValgrindModel::resetImple()
{
//    qDeleteAll(errors);
    m_errors.clear();
    reset();
}

void ValgrindModel::newDataImple(ValgrindModel::eElementType e, QString name, QString value)
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

void ValgrindModel::incomingData(QString, QString)
{
}

#include "memcheckmodel.moc"

