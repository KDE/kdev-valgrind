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

#pragma once

#include <QAbstractTableModel>

class QItemSelectionModel;

namespace valgrind
{

class CallgrindCallFunction;

class CallgrindCallInformation
{
public:
    explicit CallgrindCallInformation(const QStringList& stringValues);

    CallgrindCallFunction* caller = nullptr;
    CallgrindCallFunction* callee = nullptr;

    int callCount = 0;

    int eventValue(int type);

private:
    QVector<int> m_eventValues;
};

class CallgrindCallFunction
{
public:
    explicit CallgrindCallFunction(int eventsCount);

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

class CallgrindModel : public QAbstractTableModel
{
public:
    CallgrindModel();
    ~CallgrindModel() override;

    const QStringList& eventTypes();
    void setEventTypes(const QStringList& eventTypes);

    int currentEventType();
    void setCurrentEventType(int type);

    void setEventTotals(const QStringList& stringValues);

    void setPercentageValues(bool value);

    CallgrindCallFunction* addFunction(const QString& name,
                                       const QString& sourceFile,
                                       const QString& binaryFile);


    void addCall(CallgrindCallFunction* caller,
                 CallgrindCallFunction* callee,
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

    QList<CallgrindCallFunction*> m_functions;
    QList<CallgrindCallInformation*> m_information;
};

class FunctionEventsModel : public QAbstractTableModel
{
public:
    explicit FunctionEventsModel(CallgrindModel* baseModel);
    ~FunctionEventsModel() override;

    void setFunction(CallgrindCallFunction* function);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    CallgrindModel* m_baseModel;
    CallgrindCallFunction* m_function;
};

class FunctionCallersCalleesModel : public QAbstractTableModel
{
public:
    FunctionCallersCalleesModel(CallgrindModel* baseModel, bool isCallerModel);
    ~FunctionCallersCalleesModel() override;

    void setFunction(CallgrindCallFunction* function);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    CallgrindModel* m_baseModel;
    bool m_isCallerModel;
    CallgrindCallFunction* m_function;
};

}
