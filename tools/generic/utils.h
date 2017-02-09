/* This file is part of KDevelop
   Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#pragma once

#include <QObject>

class QFile;
class QProcess;
class QUrl;
class QWidget;

namespace Valgrind
{

/**
 * This class is used for tools : massif, callgrind.
 * It permits to remove the generated output file when the reading process
 * (massif visualizer, kcachegrind) has been killed
 */
class QFileProxyRemove : public QObject // FIXME rename
{
    Q_OBJECT

public:
    QFileProxyRemove(const QString& programPath,
                     const QStringList& args,
                     const QString& fileName,
                     QObject* parent = nullptr);
    ~QFileProxyRemove() override;

private:
    QFile* m_file;
    QProcess* m_process;
    QString m_execPath;
};

// FIXME rename (also move from nested namespace)
namespace StatJob
{

bool jobExists(const QUrl& url, QWidget* parent);

}

}
