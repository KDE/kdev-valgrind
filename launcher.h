/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>

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

#ifndef VALGRINDLAUNCHER_H_
#define VALGRINDLAUNCHER_H_



#include <interfaces/launchconfigurationpage.h>
#include <interfaces/ilauncher.h>
#include <interfaces/ilaunchmode.h>
#include "config.h"

class KJob;
class KIcon;

namespace valgrind
{
  class Plugin;

  class Launcher : public KDevelop::ILauncher
  {

  public:
      Launcher(valgrind::Plugin *inst);
      virtual QList< KDevelop::LaunchConfigurationPageFactory* > configPages() const;
      void addMode( valgrind::LaunchMode* mode );
      virtual QString description() const;
      virtual QString id();
      virtual QString name() const;
      virtual KJob* start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg);
      virtual QStringList supportedModes() const;

  private:
      QList<KDevelop::LaunchConfigurationPageFactory*> factories;
      QMap<QString, valgrind::LaunchMode*> modes;
      valgrind::Plugin * m_plugin;
  };

}

#endif /* VALGRINDLAUNCHER_H_ */
