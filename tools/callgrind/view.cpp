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
#include "model.h"

#include <QSortFilterProxyModel>
#include <QStandardItemModel>

namespace valgrind
{

CallgrindView::CallgrindView(CallgrindModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
{
    ui = new Ui::CallgrindView();
    ui->setupUi(this);

    m_callerProxyModel = new QSortFilterProxyModel(this);
    m_callerProxyModel->setDynamicSortFilter(true);
    ui->CallerListView->setModel(m_callerProxyModel);

    m_calleeProxyModel = new QSortFilterProxyModel(this);
    m_calleeProxyModel->setDynamicSortFilter(true);
    ui->CalleeListView->setModel(m_calleeProxyModel);

    m_informationDisplayMode = CallgrindCallstackItem::E_PERCENT;

    connect(ui->percentInformationEnabled, &QCheckBox::clicked, this, &CallgrindView::percentInformationClicked);

    m_functionListSelectedItem = nullptr;

//     QItemSelectionModel *selectionModel = new QItemSelectionModel(m_callerProxyModel);
//     ui->CallerListView->setSelectionModel(selectionModel);
//
//     connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
//             this, SLOT(selectionOnCallerListChanged(const QItemSelection &, const QItemSelection &)));
//
//     connect(this, SIGNAL(activated(QModelIndex)), SLOT(openDocument(QModelIndex)));

    Q_ASSERT(m_model);
    m_model->setParent(this);

    auto fctTreeModel = m_model->abstractItemModel(CallgrindModel::E_FCT_LIST);
    ui->FunctionsTreeView->setSortingEnabled(true);
    ui->FunctionsTreeView->setModel(fctTreeModel);

    auto functionListSelectionModel = new QItemSelectionModel(fctTreeModel);
    ui->FunctionsTreeView->setSelectionModel(functionListSelectionModel);
    connect(functionListSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &CallgrindView::selectionOnFctListChanged);

    ui->FunctionsTreeView->header()->setStretchLastSection(false);
    ui->FunctionsTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

CallgrindView::~CallgrindView()
{
    delete ui;
}

void CallgrindView::selectionOnFctListChanged(const QItemSelection& selected, const QItemSelection&)
{
    auto item = static_cast<CallgrindCallstackItem*>(selected.indexes().first().internalPointer());
    m_functionListSelectedItem = item;

    if (!m_functionListSelectedItem) {
        return;
    }

    updateInformationTab(item);
    updateCallerTab(item);
    updateCalleeTab(item);
}

void CallgrindView::updateInformationTab(CallgrindCallstackItem* item)
{
    if (ui->InformationsListView->model()) {
        delete ui->InformationsListView->model();
    }

    auto informationsModel = new QStandardItemModel(this);
    for (int i = 0; i < (CallgrindItem::Unknow - 1); ++i) {
        if (item->hasKey(i)) {
            informationsModel->setItem(i, 0, new QStandardItem(m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()));
            informationsModel->setItem(i, 1, new QStandardItem(item->data((CallgrindItem::Columns) i, m_informationDisplayMode).toString()));
        }
    }

    ui->InformationsListView->setModel(informationsModel);
    ui->InformationsListView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void CallgrindView::updateCallerTab(CallgrindCallstackItem* item)
{
    if (m_callerProxyModel->sourceModel()) {
        delete m_callerProxyModel->sourceModel();
    }

    auto callerModel = new QStandardItemModel(this);
    callerModel->setHorizontalHeaderItem(0, new QStandardItem(i18n("Function Name")));
    callerModel->setHorizontalHeaderItem(1, new QStandardItem (i18n("Ir")));

    for (int i = 0; i < item->parentCount(); ++i) {
        auto caller = item->parent(i);
        callerModel->setItem(i, 0, new QStandardItem(caller->csFunction()->functionName()));
        callerModel->setItem(i, 1, new QStandardItem(caller->data(CallgrindItem::InstructionRead, CallgrindCallstackItem::E_PERCENT).toString()));
    }

    m_callerProxyModel->setSourceModel(callerModel);

    ui->CallerListView->header()->setStretchLastSection(false);
    ui->CallerListView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void CallgrindView::updateCalleeTab(CallgrindCallstackItem* item)
{
    if (m_calleeProxyModel->sourceModel()) {
        delete m_calleeProxyModel->sourceModel();
    }

    auto calleeModel = new QStandardItemModel(this);
    calleeModel->setHorizontalHeaderItem(0, new QStandardItem(i18n("Function Name")));
    calleeModel->setHorizontalHeaderItem(1, new QStandardItem(i18n("Ir")));

    for (int i = 0; i < item->childCount(); ++i) {
        auto callee = item->child(i);
        calleeModel->setItem(i, 0, new QStandardItem(callee->csFunction()->functionName()));
        calleeModel->setItem(i, 1, new QStandardItem(callee->data(CallgrindItem::InstructionRead, CallgrindCallstackItem::E_PERCENT).toString()));
    }

    m_calleeProxyModel->setSourceModel(calleeModel);

    ui->CalleeListView->header()->setStretchLastSection(false);
    ui->CalleeListView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void CallgrindView::percentInformationClicked(bool enable)
{
    if (enable) {
        m_informationDisplayMode = CallgrindCallstackItem::E_PERCENT;
    } else {
        m_informationDisplayMode = CallgrindCallstackItem::E_NORMAL;
    }

    if (!m_functionListSelectedItem) {
        return;
    }

    updateInformationTab(m_functionListSelectedItem);
}

void CallgrindView::selectionOnCallerListChanged(const QItemSelection& s, const QItemSelection&)
{
    qCDebug(KDEV_VALGRIND) << "select";

    auto item = static_cast<CallgrindCallstackItem*>(s.indexes().first().internalPointer());
    if (!item) {
        qCDebug(KDEV_VALGRIND) << "NULL";
        return;
    }

    for (int i = 0; i < (CallgrindItem::Unknow - 1); ++i) {
        if (item->hasKey(i)) {
            qCDebug(KDEV_VALGRIND) << m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()
                                   << item->data((CallgrindItem::Columns)i).toString();
        }
    }
}

}
