/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "marks.h"

#include "modelwrapper.h"
#include "plugin.h"

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/MarkInterface>

#include <QAbstractItemModel>

namespace valgrind
{

Marks::Marks(Plugin* plugin)
    : m_plugin(plugin)
    , m_model(0)
{
    connect(plugin, &Plugin::newModel, this, &Marks::newModel);
}

Marks::~Marks()
{
}

void Marks::newModel(Model* model)
{
    m_model = model;
    connect(m_model->getModelWrapper(), &ModelWrapper::modelChanged, this, &Marks::modelChanged);
}

void Marks::modelChanged()
{
    // parse model to display errors in the editor
    auto documents = KTextEditor::Editor::instance()->documents();
    foreach (auto document, documents) {
        auto iface = qobject_cast<KTextEditor::MarkInterface*>(document);
        if (iface)
            iface->clearMarks();
    }

    QModelIndex parentIndex = QModelIndex();
    QAbstractItemModel* itemModel = m_model->getQAbstractItemModel();
    if(!itemModel)
        return;

    // errors
    int numRows = itemModel->rowCount();
    for (int row = 0; row < numRows; ++row) {
        QModelIndex index = itemModel->index(row, 0, parentIndex);

        // stacks
        int numRows2 = itemModel->rowCount(index);
        for (int row2 = 0; row2 < numRows2; ++row2) {
            QModelIndex index2 = itemModel->index(row2, 0, index);

            // frames
            int numRows3 = itemModel->rowCount(index2);
            for (int row3 = 0; row3 < numRows3; ++row3) {
                QModelIndex index3 = itemModel->index(row3, 0, index2);

                QStringList fileInfo = itemModel->data(index3, Qt::UserRole).toString().split(":");
                if (fileInfo.size() != 2)
                    continue;

                foreach (auto document, documents) {
                    auto iface = qobject_cast<KTextEditor::MarkInterface*>(document);
                    if (document->documentName() == fileInfo[0] && iface)
                        iface->addMark(fileInfo[1].toInt() - 1, KTextEditor::MarkInterface::markType07);
                }
            }
        }
    }
}

}
