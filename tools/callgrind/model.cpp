/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "model.h"

#include "debug.h"
#include "generic/events.h"

#include <klocalizedstring.h>

#include <QItemSelectionModel>

namespace valgrind
{

static const int rightAlign = int(Qt::AlignRight | Qt::AlignVCenter);

void addValues(const QStringList& stringValues, QVector<int>& intValues)
{
    for (int i = 0; i < stringValues.size(); ++i) {
        intValues[i] += stringValues[i].toInt();
    }
}

void emitDataChanged(QAbstractTableModel* model)
{
    Q_ASSERT(model);

    emit model->dataChanged(model->index(0,0),
                            model->index(model->rowCount() - 1, model->columnCount() - 1),
                            { Qt::DisplayRole });
}

CallgrindCallInformation::CallgrindCallInformation(const QStringList& stringValues)
{
    Q_ASSERT(!stringValues.isEmpty());

    m_eventValues.resize(stringValues.size());
    addValues(stringValues, m_eventValues);
}

int CallgrindCallInformation::eventValue(int type)
{
    Q_ASSERT(type < m_eventValues.size());
    return m_eventValues[type];
}

CallgrindCallFunction::CallgrindCallFunction(int eventsCount)
{
    m_eventValues.resize(eventsCount);
}

int CallgrindCallFunction::callCount()
{
    int count = 0;
    foreach (auto info, callersInformation) {
        count += info->callCount;
    }
    return count;
}

int CallgrindCallFunction::eventValue(int type, bool inclusive)
{
    Q_ASSERT(type < m_eventValues.size());

    int value = m_eventValues[type];
    if (!inclusive) {
        return value;
    }

    if (callersInformation.isEmpty()) {
        // The function is NOT CALLED by others, therefore we calc
        // the event inclusive value as sum of self value and all callees.
        foreach (auto info, calleesInformation) {
            value += info->eventValue(type);
        }
        return value;
    }

    // The function is CALLED by others, therefore we calc
    // the event inclusive value as sum of all callers.
    value = 0;
    foreach (auto info, callersInformation) {
        value += info->eventValue(type);
    }

    return value;
}

void CallgrindCallFunction::addEventValues(const QStringList& stringValues)
{
    Q_ASSERT(stringValues.size() == m_eventValues.size());
    addValues(stringValues, m_eventValues);
}

CallgrindModel::CallgrindModel()
    : m_currentEventType(0)
    , m_percentageValues(false)
{
}

CallgrindModel::~CallgrindModel()
{
    qDeleteAll(m_functions);
    qDeleteAll(m_information);
}

const QStringList & CallgrindModel::eventTypes()
{
    return m_eventTypes;
}

void CallgrindModel::setEventTypes(const QStringList& eventTypes)
{
    Q_ASSERT(!eventTypes.isEmpty());
    m_eventTypes = eventTypes;
    m_eventTotals.resize(m_eventTypes.size());
}

void CallgrindModel::setEventTotals(const QStringList& stringValues)
{
    Q_ASSERT(stringValues.size() == m_eventTotals.size());
    addValues(stringValues, m_eventTotals);
    qDebug() << m_eventTotals;
}

int CallgrindModel::currentEventType()
{
    return m_currentEventType;
}

void CallgrindModel::setCurrentEventType(int type)
{
    Q_ASSERT(type < m_eventTotals.size());

    m_currentEventType = type;
    emitDataChanged(this);
}

void CallgrindModel::setPercentageValues(bool value)
{
    m_percentageValues = value;
    emitDataChanged(this);
}

CallgrindCallFunction* CallgrindModel::addFunction(
    const QString& name,
    const QString& sourceFile,
    const QString& binaryFile)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(!m_eventTypes.isEmpty());

    CallgrindCallFunction* function = nullptr;

    foreach (CallgrindCallFunction* currentFunction, m_functions) {
        if (currentFunction->name == name && (currentFunction->binaryFile.isEmpty() ||
                                              binaryFile.isEmpty() ||
                                              currentFunction->binaryFile == binaryFile)) {
            function = currentFunction;
            break;
        }
    }

    if (function) {
        if (function->binaryFile.isEmpty()) {
            function->binaryFile = binaryFile;
        }

        function->sourceFiles += sourceFile;
        function->sourceFiles.removeDuplicates();
        function->sourceFiles.sort();
    }

    else {
        function = new CallgrindCallFunction(m_eventTypes.size());

        function->name = name;
        function->binaryFile = binaryFile;
        function->sourceFiles += sourceFile;

        m_functions.append(function);
    }

    return function;
}

void CallgrindModel::addCall(
    CallgrindCallFunction* caller,
    CallgrindCallFunction* callee,
    int callCount,
    const QStringList& eventValues)
{
    Q_ASSERT(caller);
    Q_ASSERT(callee);

    auto info = new CallgrindCallInformation(eventValues);
    m_information.append(info);

    info->caller = caller;
    info->callee = callee;
    info->callCount = callCount;

    caller->calleesInformation.append(info);
    callee->callersInformation.append(info);
}

QModelIndex CallgrindModel::index(int row, int column, const QModelIndex&) const
{
    if (hasIndex(row, column)) {
        return createIndex(row, column, m_functions.at(row));
    }

    return QModelIndex();
}

int CallgrindModel::rowCount(const QModelIndex&) const
{
    return m_functions.size();
}

int CallgrindModel::columnCount(const QModelIndex&) const
{
    return 4;
}

QString CallgrindModel::displayValue(int value) const
{
    QString result = QString::number(value);
    int length = result.length();

    for (int i = 0; i < (length / 3); ++i) {
        int pos = result.length() - (4 * i + 3);
        if (!pos) {
            break;
        }
        result.insert(pos, ' ');
    }

    return result;
}

QString CallgrindModel::displayValue(double value) const
{
    return QString::number(value, 'f', 2);
}

QString CallgrindModel::displayValue(int eventIntValue, int eventType) const
{
    if (m_percentageValues) {
        return displayValue(eventIntValue * 100.0 / m_eventTotals[eventType]);
    }

    return displayValue(eventIntValue);
}


QVariant CallgrindModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto function = static_cast<CallgrindCallFunction*>(index.internalPointer());

    if (role == Qt::TextAlignmentRole && index.column() < 3) {
        return rightAlign;
    }

    if (index.column() < 2) {
        int intValue = function->eventValue(m_currentEventType, (index.column() == 0));

        if (role == SortRole) {
            return intValue;
        }

        if (role == Qt::DisplayRole) {
            return displayValue(intValue, m_currentEventType);
        }
    }

    if (index.column() == 2 && (role == Qt::DisplayRole || role == SortRole)) {
        return function->callCount();
    }

    if (index.column() == 3 && (role == Qt::DisplayRole || role == SortRole)) {
        return function->name;
    }

    return QVariant();
}

QVariant CallgrindModel::headerData(int section, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (section) {
            case 0: return i18n("Incl.");
            case 1: return i18n("Self");
            case 2: return i18n("Called");
            case 3: return i18n("Function");
        }
    }

    return QVariant();
}

FunctionEventsModel::FunctionEventsModel(CallgrindModel* baseModel)
    : QAbstractTableModel(baseModel)
    , m_baseModel(baseModel)
    , m_function(nullptr)
{
    Q_ASSERT(m_baseModel);

    connect(m_baseModel, &CallgrindModel::dataChanged,
            this, [this](const QModelIndex&, const QModelIndex&, const QVector<int>&) {

        emitDataChanged(this);
    });
}

FunctionEventsModel::~FunctionEventsModel()
{
}

void FunctionEventsModel::setFunction(valgrind::CallgrindCallFunction* function)
{
    m_function = function;
    emitDataChanged(this);
}

QModelIndex FunctionEventsModel::index(int row, int column, const QModelIndex&) const
{
    if (hasIndex(row, column)) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

int FunctionEventsModel::rowCount(const QModelIndex&) const
{
    return m_baseModel->eventTypes().size();
}

int FunctionEventsModel::columnCount(const QModelIndex&) const
{
    return 4;
}

QVariant FunctionEventsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (column == 0 && role == Qt::DisplayRole) {
        return eventFullName(m_baseModel->eventTypes().at(row));
    }

    if (column == 3 && role == Qt::DisplayRole) {
        return m_baseModel->eventTypes().at(row);
    }

    if (m_function && (column == 1 || column == 2)) {
        if (role == Qt::TextAlignmentRole) {
            return rightAlign;
        }

        int intValue = m_function->eventValue(row, (column == 1));

        if (role == CallgrindModel::SortRole) {
            return intValue;
        }

        if (role == Qt::DisplayRole) {
            return m_baseModel->displayValue(intValue, row);
        }
    }

    return QVariant();
}

QVariant FunctionEventsModel::headerData(int section, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (section) {
            case 0: return i18n("Event");
            case 1: return i18n("Incl.");
            case 2: return i18n("Self");
            case 3: return i18n("Short");
        }
    }

    return QVariant();
}

FunctionCallersCalleesModel::FunctionCallersCalleesModel(CallgrindModel* baseModel, bool isCallerModel)
    : QAbstractTableModel(baseModel)
    , m_baseModel(baseModel)
    , m_isCallerModel(isCallerModel)
    , m_function(nullptr)
{
    Q_ASSERT(m_baseModel);

    connect(m_baseModel, &CallgrindModel::dataChanged,
            this, [this](const QModelIndex&, const QModelIndex&, const QVector<int>&) {

        emitDataChanged(this);
        emit headerDataChanged(Qt::Horizontal, 0, 1);
    });
}

FunctionCallersCalleesModel::~FunctionCallersCalleesModel()
{
}

void FunctionCallersCalleesModel::setFunction(valgrind::CallgrindCallFunction* function)
{
    beginResetModel();
    m_function = function;
    endResetModel();
}

QModelIndex FunctionCallersCalleesModel::index(int row, int column, const QModelIndex&) const
{
    if (hasIndex(row, column)) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

int FunctionCallersCalleesModel::rowCount(const QModelIndex&) const
{
    if (!m_function) {
        return 0;
    }

    if (m_isCallerModel) {
        return m_function->callersInformation.size();
    }

    return m_function->calleesInformation.size();
}

int FunctionCallersCalleesModel::columnCount(const QModelIndex&) const
{
    return 4;
}

QVariant FunctionCallersCalleesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (role == Qt::TextAlignmentRole && column < 3) {
        return rightAlign;
    }

    auto info = m_isCallerModel ? m_function->callersInformation.at(row) : m_function->calleesInformation.at(row);
    int eventType = m_baseModel->currentEventType();
    int intValue = info->eventValue(eventType);
    int callCount = info->callCount;

    if (column == 0) {
        if (role == CallgrindModel::SortRole) {
            return intValue;
        }

        if (role == Qt::DisplayRole) {
            return m_baseModel->displayValue(intValue, eventType);
        }
    }

    if (column == 1) {
        int perCallValue = intValue / callCount;

        if (role == CallgrindModel::SortRole) {
            return perCallValue;
        }

        if (role == Qt::DisplayRole) {
            return m_baseModel->displayValue(perCallValue);
        }
    }

    if (column == 2) {
        if (role == CallgrindModel::SortRole || role == Qt::DisplayRole) {
            return callCount;
        }
    }

    if (column == 3) {
        if (role == CallgrindModel::SortRole || role == Qt::DisplayRole) {
            if (m_isCallerModel) {
                return info->caller->name;
            }
            return info->callee->name;
        }
    }

    return QVariant();
}

QVariant FunctionCallersCalleesModel::headerData(int section, Qt::Orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        const QString& eventType = m_baseModel->eventTypes().at(m_baseModel->currentEventType());

        switch (section) {
            case 0: return eventType;
            case 1: return i18n("%1 per call", eventType);
            case 2: return i18n("Count");
            case 3:
                if (m_isCallerModel) {
                    return i18n("Caller");
                }
                return i18n("Callee");
        }
    }

    return QVariant();
}

}
