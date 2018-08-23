/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "utils.h"

#include <KParts/MainWindow>
#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>

namespace Valgrind
{

QString eventFullName(const QString& eventShortName)
{
    static QMap<QString, QString> fullNames;
    static bool initDone = false;
    if (!initDone) {
        initDone = true;

        // full names are from KCachegrind

        fullNames["Ir"  ] = i18n("Instruction Fetch");
        fullNames["Dr"  ] = i18n("Data Read Access");
        fullNames["Dw"  ] = i18n("Data Write Access");
        fullNames["I1mr"] = i18n("L1 Instr. Fetch Miss");
        fullNames["D1mr"] = i18n("L1 Data Read Miss");
        fullNames["D1mw"] = i18n("L1 Data Write Miss");
        fullNames["ILmr"] = i18n("LL Instr. Fetch Miss");
        fullNames["DLmr"] = i18n("LL Data Read Miss");
        fullNames["DLmw"] = i18n("LL Data Write Miss");

        fullNames["Bc"  ] = i18n("Conditional Branch");
        fullNames["Bcm" ] = i18n("Mispredicted Cond. Branch");
        fullNames["Bi"  ] = i18n("Indirect Branch");
        fullNames["Bim" ] = i18n("Mispredicted Ind. Branch");
    }

    return fullNames.value(eventShortName, eventShortName);
}

QString displayValue(int value)
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

QString displayValue(double value)
{
    return QString::number(value, 'f', 2);
}

void emitDataChanged(QAbstractTableModel* model)
{
    Q_ASSERT(model);

    emit model->dataChanged(model->index(0,0),
                            model->index(model->rowCount() - 1, model->columnCount() - 1),
                            { Qt::DisplayRole });
}

void setupVisualizerProcess(
    QProcess* visualizerProcess,
    QPushButton* startButton,
    std::function<void()> startFunction,
    bool startImmediately)
{
    Q_ASSERT(visualizerProcess);
    Q_ASSERT(startButton);

    QObject::connect(startButton, &QPushButton::clicked,
                     startButton, [startFunction]() {
                         startFunction();
                    });

    QObject::connect(visualizerProcess, &QProcess::started, startButton, [startButton]() {
        startButton->setEnabled(false);
    });

    QObject::connect(visualizerProcess, &QProcess::errorOccurred, startButton, [visualizerProcess, startButton]() {
        QString errorMessage;
        if (visualizerProcess->error() == QProcess::FailedToStart) {
            errorMessage += i18n("Failed to start visualizer from \"%1\".", visualizerProcess->program());
            errorMessage += QStringLiteral("\n\n");
            errorMessage += i18n("Check your settings and install the visualizer if necessary.");
        } else {
            errorMessage += i18n("Error during visualizer execution:");
            errorMessage += QStringLiteral("\n\n");
            errorMessage += visualizerProcess->errorString();
        }
        KMessageBox::error(activeMainWindow(), errorMessage, i18n("Valgrind Error"));

        startButton->setEnabled(true);
    });

    QObject::connect(visualizerProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     startButton, [startButton]() {
                         startButton->setEnabled(true);
                    });

    if (startImmediately) {
        startFunction();
    }
}

QWidget* activeMainWindow()
{
    return KDevelop::ICore::self()->uiController()->activeMainWindow();
}

QString findExecutable(const QString &executableName)
{
    QString executablePath = QStandardPaths::findExecutable(executableName);
    return executablePath.isEmpty() ? executableName : executablePath;
}

}
