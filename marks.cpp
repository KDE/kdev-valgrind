/* This file is part of KDevelop
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

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/MarkInterface>
#include <QAbstractItemModel>

#include "marks.h"
#include "memcheckmodel.h"
#include "plugin.h"
#include "modelwrapper.h"


namespace valgrind
{

Marks::Marks(valgrind::Plugin *plugin)
    : m_plugin(plugin)
    , m_model(0)
{
    connect(plugin, SIGNAL(newModel(valgrind::Model*)), this, SLOT(newModel(valgrind::Model*)));
}

Marks::~Marks()
{
}


void Marks::newModel(valgrind::Model* model)
{
    m_model = model;
    connect(m_model->getModelWrapper(), SIGNAL(modelChanged()),
            this, SLOT(modelChanged()));
}

void Marks::modelChanged()
{
    // parse model to display errors in the editor
    KTextEditor::Editor* editor = KTextEditor::editor("katepart");
    QList<KTextEditor::Document*> docList = editor->documents();
    for (int i = 0; i < docList.size(); ++i)
        if (KTextEditor::MarkInterface *iface = qobject_cast<KTextEditor::MarkInterface*>(docList.at(i)))
            iface->clearMarks();
    // errors
    QModelIndex parentIndex = QModelIndex();
    QAbstractItemModel *itemModel = m_model->getQAbstractItemModel();
    int numRows = itemModel->rowCount();
    for (int row = 0; row < numRows; ++row) {
        QModelIndex index = itemModel->index(row, 0, parentIndex);
        int numRows2 = itemModel->rowCount(index);
        for (int row2 = 0; row2 < numRows2; ++row2) {
            QModelIndex index2 = itemModel->index(row2, 0, index);
            QString text = itemModel->data(index2, Qt::UserRole).toString();
            if (!text.isEmpty()) {
                QString delimiterPattern(":");
                QStringList fileInfo = text.split(delimiterPattern);
                if (fileInfo.size() == 2)
                    for (int i = 0; i < docList.size(); ++i)
                        if (docList.at(i)->documentName() == fileInfo[0])
                            if (KTextEditor::MarkInterface *iface = qobject_cast<KTextEditor::MarkInterface*>(docList.at(i)))
                                iface->addMark(fileInfo[1].toInt() - 1, KTextEditor::MarkInterface::markType07);
            }
        }
    }
}

}

#include "marks.moc"
