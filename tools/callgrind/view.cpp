/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>

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

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "statjob.h"

#include "debug.h"
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "model.h"
#include "modelitem.h"

#include <QResizeEvent>

namespace valgrind
{
CallgrindView::CallgrindView(QWidget *parent) : QWidget(parent)
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
/*
    QItemSelectionModel *selectionModel = new QItemSelectionModel(m_callerProxyModel);
    ui->CallerListView->setSelectionModel(selectionModel);

    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionOnCallerListChanged(const QItemSelection &, const QItemSelection &)));
*/
    //connect(this, SIGNAL(activated(QModelIndex)), SLOT(openDocument(QModelIndex)));
}

CallgrindView::~CallgrindView()
{
}

void CallgrindView::setModel(Model * m)
{
    m_model = dynamic_cast<CallgrindModel*>(m);
    QAbstractItemModel  *fctTreeModel = m->getQAbstractItemModel(CallgrindModel::E_FCT_LIST);
    ui->FunctionsTreeView->setSortingEnabled(true);
    ui->FunctionsTreeView->setModel( fctTreeModel );

    QItemSelectionModel *functionListSelectionModel = new QItemSelectionModel(fctTreeModel);
    ui->FunctionsTreeView->setSelectionModel(functionListSelectionModel);
    connect(functionListSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &CallgrindView::selectionOnFctListChanged);

    ui->FunctionsTreeView->header()->setStretchLastSection(false);
    ui->FunctionsTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void CallgrindView::selectionOnFctListChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    CallgrindCallstackItem  *item = static_cast<CallgrindCallstackItem*>(selected.indexes().first().internalPointer());
    m_functionListSelectedItem = item;

    if (m_functionListSelectedItem == nullptr)
        return ;

    updateInformationTab(item);
    updateCallerTab(item);
    updateCalleeTab(item);
}

void CallgrindView::updateInformationTab(CallgrindCallstackItem *item)
{
    if (ui->InformationsListView->model()!= nullptr)
        delete ui->InformationsListView->model();
    QStandardItemModel *informationsModel = new QStandardItemModel(this);
    int size = ((int) iCachegrindItem::Unknow) - 1;
    for (int i = 0; i < size; ++i)
    {
        if (item->hasKey(i))
        {
            informationsModel->setItem(i, 0, new QStandardItem(m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()));
            informationsModel->setItem(i, 1, new QStandardItem(item->data((iCachegrindItem::Columns) i, m_informationDisplayMode).toString()));
        }
    }
    ui->InformationsListView->setModel(informationsModel);
    ui->InformationsListView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void CallgrindView::updateCallerTab(CallgrindCallstackItem *item)
{
    //delete old model
    if (m_callerProxyModel->sourceModel() != nullptr)
        delete m_callerProxyModel->sourceModel();
    QStandardItemModel    *callerModel = new QStandardItemModel(this);
    callerModel->setHorizontalHeaderItem(0, new QStandardItem(i18n("Function Name")));
    callerModel->setHorizontalHeaderItem(1, new QStandardItem (i18n("Ir")));
    int callerSize = item->getParentCount();
    for (int i = 0; i < callerSize; ++i)
    {
        CallgrindCallstackItem *caller = item->getParent(i);
        callerModel->setItem(i, 0, new QStandardItem(caller->getCsFunction()->getFunctionName()));
        callerModel->setItem(i, 1, new QStandardItem(caller->data(iCachegrindItem::InstructionRead, CallgrindCallstackItem::E_PERCENT).toString()));
    }
    m_callerProxyModel->setSourceModel(callerModel);

    ui->CallerListView->header()->setStretchLastSection(false);
    ui->CallerListView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void CallgrindView::updateCalleeTab(CallgrindCallstackItem *item)
{
    if (m_calleeProxyModel->sourceModel() != nullptr)
        delete m_calleeProxyModel->sourceModel();
    QStandardItemModel  *calleeModel = new QStandardItemModel(this);
    calleeModel->setHorizontalHeaderItem(0, new QStandardItem(i18n("Function Name")));
    calleeModel->setHorizontalHeaderItem(1, new QStandardItem(i18n("Ir")));
    int calleeSize = item->getChildCount();
    for (int i = 0; i < calleeSize; ++i)
    {
        CallgrindCallstackItem *callee = item->getChild(i);
        calleeModel->setItem(i, 0, new QStandardItem(callee->getCsFunction()->getFunctionName()));
        calleeModel->setItem(i, 1, new QStandardItem(callee->data(iCachegrindItem::InstructionRead, CallgrindCallstackItem::E_PERCENT).toString()));
    }
    m_calleeProxyModel->setSourceModel(calleeModel);

    ui->CalleeListView->header()->setStretchLastSection(false);
    ui->CalleeListView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void CallgrindView::percentInformationClicked(bool enable)
{
    m_informationDisplayMode = CallgrindCallstackItem::E_PERCENT;
    if (!enable)
        m_informationDisplayMode = CallgrindCallstackItem::E_NORMAL;
    if (m_functionListSelectedItem == nullptr)
        return ;
    updateInformationTab(m_functionListSelectedItem);
}

void CallgrindView::selectionOnCallerListChanged(const QItemSelection &s, const QItemSelection &d)
{
    Q_UNUSED(d);
    qCDebug(KDEV_VALGRIND) << "select";
    CallgrindCallstackItem  *item = static_cast<CallgrindCallstackItem*>(s.indexes().first().internalPointer());
    if (item == nullptr)
    {
        qCDebug(KDEV_VALGRIND) << "NULL";
        return;
    }
    int size = ((int) iCachegrindItem::Unknow) - 1;
    for (int i = 0; i < size; ++i)
    {
        if (item->hasKey(i))
        {
            qCDebug(KDEV_VALGRIND) << m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() <<
                        item->data((iCachegrindItem::Columns) i).toString();
        }
    }
}

Model * CallgrindView::model(void)
{
    return m_model;
}

void CallgrindView::WidgetContainerResizeEvent( QResizeEvent *event )
{
    int heightDelta = event->oldSize().height() - this->height();
    int widthDelta = event->oldSize().width() - this->width();

    this->resize(event->size().width() - widthDelta, event->size().height() - heightDelta);
}

}

