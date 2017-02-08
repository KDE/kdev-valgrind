/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 *  Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "view.h"
#include "ui_view.h"

#include "debug.h"
#include "generic/utils.h"
#include "generic/model.h"
#include "model.h"

#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include <QApplication>
#include <QSortFilterProxyModel>

namespace valgrind
{

CachegrindView::CachegrindView(CachegrindModel* model, QWidget* parent)
    : QWidget(parent)
{
    Q_ASSERT(model);
    model->setParent(this);

    ui = new Ui::CachegrindView;
    ui->setupUi(this);

    connect(ui->percenageValues, &QCheckBox::stateChanged,
            model, &CachegrindModel::setPercentageValues);
    model->setPercentageValues(ui->percenageValues->checkState());

    auto functionsProxyModel = new QSortFilterProxyModel(this);
    functionsProxyModel->setSourceModel(model);
    functionsProxyModel->setSortRole(SortRole);
    functionsProxyModel->setFilterKeyColumn(-1);
    ui->functionsView->setModel(functionsProxyModel);
    ui->functionsView->setSortingEnabled(true);
    ui->functionsView->sortByColumn(1);

    ui->functionsView->header()->resizeSections(QHeaderView::ResizeToContents);
    ui->functionsView->header()->setStretchLastSection(false);
    ui->functionsView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(ui->searchEdit, &QLineEdit::textChanged,
            functionsProxyModel, &QSortFilterProxyModel::setFilterWildcard);

    connect(ui->functionsView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [=](const QModelIndex& currentProxyIndex, const QModelIndex&) {

        auto sourceIndex = functionsProxyModel->mapToSource(currentProxyIndex);
        auto item = static_cast<CachegrindItem*>(sourceIndex.internalPointer());

        if (item) {
            ui->nameLabel->setText(item->functionName);
            ui->sourceLabel->setText(item->fileName);
        } else {
            ui->nameLabel->clear();
            ui->sourceLabel->clear();
        }
    });
    ui->nameLabel->clear();
    ui->sourceLabel->clear();

    connect(ui->functionsView, &QTreeView::activated, this, [this](const QModelIndex& index) {
        auto item = static_cast<CachegrindItem*>(index.internalPointer());
        QUrl doc = QUrl::fromLocalFile(item->fileName);
        if (doc.isValid() && StatJob::jobExists(doc, qApp->activeWindow())) {
            KDevelop::ICore::self()->documentController()->openDocument(doc);
        }
    });
}

CachegrindView::~CachegrindView()
{
    delete ui;
}

}
