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
#include "generic/events.h"
#include "generic/model.h"
#include "model.h"

#include <QSortFilterProxyModel>

namespace valgrind
{

CallgrindView::CallgrindView(CallgrindModel* model, QWidget* parent)
    : QWidget(parent)
{
    Q_ASSERT(model);
    model->setParent(this);

    ui = new Ui::CallgrindView();
    ui->setupUi(this);

    foreach (const QString& eventType, model->eventTypes()) {
        ui->eventTypes->addItem(eventFullName(eventType));
    }
    connect(ui->eventTypes, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            model, &CallgrindModel::setCurrentEventType);
    model->setCurrentEventType(ui->eventTypes->currentIndex());

    connect(ui->percenageValues, &QCheckBox::stateChanged,
            model, &CallgrindModel::setPercentageValues);
    model->setPercentageValues(ui->percenageValues->checkState());

    auto functionsProxyModel = new QSortFilterProxyModel(this);
    functionsProxyModel->setSourceModel(model);
    functionsProxyModel->setSortRole(SortRole);
    functionsProxyModel->setFilterKeyColumn(-1);
    ui->functionsView->setModel(functionsProxyModel);
    ui->functionsView->setSortingEnabled(true);

    connect(ui->searchEdit, &QLineEdit::textChanged,
            functionsProxyModel, &QSortFilterProxyModel::setFilterWildcard);

    auto eventsModel = new FunctionEventsModel(model);
    ui->eventsView->setModel(eventsModel);
    ui->eventsView->header()->resizeSections(QHeaderView::ResizeToContents);

    auto callersModel = new FunctionCallersCalleesModel(model, true);
    auto callersProxyModel = new QSortFilterProxyModel(this);
    callersProxyModel->setSourceModel(callersModel);
    callersProxyModel->setSortRole(SortRole);
    ui->callersView->setModel(callersProxyModel);
    ui->callersView->setSortingEnabled(true);

    connect(callersModel, &FunctionCallersCalleesModel::headerDataChanged,
            this, [this](Qt::Orientation, int, int) {
        ui->callersView->header()->resizeSections(QHeaderView::ResizeToContents);
    });

    auto calleesModel = new FunctionCallersCalleesModel(model, false);
    auto calleesProxyModel = new QSortFilterProxyModel(this);
    calleesProxyModel->setSourceModel(calleesModel);
    calleesProxyModel->setSortRole(SortRole);
    ui->calleesView->setModel(calleesProxyModel);
    ui->calleesView->setSortingEnabled(true);

    connect(calleesModel, &FunctionCallersCalleesModel::headerDataChanged,
            this, [this](Qt::Orientation, int, int) {
        ui->calleesView->header()->resizeSections(QHeaderView::ResizeToContents);
    });

    connect(ui->functionsView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [=](const QModelIndex& currentProxyIndex, const QModelIndex&) {

        auto sourceIndex = functionsProxyModel->mapToSource(currentProxyIndex);
        auto function = static_cast<CallgrindCallFunction*>(sourceIndex.internalPointer());

        eventsModel->setFunction(function);
        callersModel->setFunction(function);
        calleesModel->setFunction(function);

        if (function) {
            ui->binaryLabel->setText(function->binaryFile);
            ui->sourceLabel->setText(function->sourceFiles.join('\n'));
        } else {
            ui->binaryLabel->clear();
            ui->sourceLabel->clear();
        }
    });
    ui->binaryLabel->clear();
    ui->sourceLabel->clear();

    if (functionsProxyModel->rowCount()) {
        ui->functionsView->setCurrentIndex(functionsProxyModel->index(0,0));
    }
}

CallgrindView::~CallgrindView()
{
    delete ui;
}

}
