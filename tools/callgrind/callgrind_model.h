/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 * Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#pragma once

#include <QAbstractTableModel>

class QItemSelectionModel;

namespace Valgrind
{

class CallgrindFunction;

class CallgrindCallInformation
{
public:
    explicit CallgrindCallInformation(const QStringList& stringValues);

    CallgrindFunction* caller = nullptr;
    CallgrindFunction* callee = nullptr;

    int callCount = 0;

    int eventValue(int type);

private:
    QVector<int> m_eventValues;
};

class CallgrindFunction
{
public:
    explicit CallgrindFunction(int eventsCount);

    QString name;
    QString binaryFile;
    QStringList sourceFiles;

    int callCount();

    int eventValue(int type, bool inclusive);
    void addEventValues(const QStringList& stringValues);

    QList<CallgrindCallInformation*> callersInformation;
    QList<CallgrindCallInformation*> calleesInformation;

private:
    QVector<int> m_eventValues;
};

class CallgrindFunctionsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CallgrindFunctionsModel();
    ~CallgrindFunctionsModel() override;

    const QStringList& eventTypes();
    void setEventTypes(const QStringList& eventTypes);

    int currentEventType();
    void setCurrentEventType(int type);

    void setEventTotals(const QStringList& stringValues);

    void setPercentageValues(bool value);

    CallgrindFunction* addFunction(
        const QString& name,
        const QString& sourceFile,
        const QString& binaryFile);


    void addCall(
        CallgrindFunction* caller,
        CallgrindFunction* callee,
        int callCount,
        const QStringList& eventValues);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QVariant eventData(int type, int intValue, int role) const;

    QString displayValue(int eventIntValue, int eventType) const;

private:
    QStringList m_eventTypes;
    int m_currentEventType;

    QVector<int> m_eventTotals;

    bool m_percentageValues;

    QList<CallgrindFunction*> m_functions;
    QList<CallgrindCallInformation*> m_information;
};

class CallgrindFunctionEventsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CallgrindFunctionEventsModel(CallgrindFunctionsModel* baseModel);
    ~CallgrindFunctionEventsModel() override;

    void setFunction(CallgrindFunction* function);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    CallgrindFunctionsModel* m_baseModel;
    CallgrindFunction* m_function;
};

class CallgrindFunctionCallersCalleesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CallgrindFunctionCallersCalleesModel(CallgrindFunctionsModel* baseModel, bool isCallerModel);
    ~CallgrindFunctionCallersCalleesModel() override;

    void setFunction(CallgrindFunction* function);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    CallgrindFunctionsModel* m_baseModel;
    bool m_isCallerModel;
    CallgrindFunction* m_function;
};

}
