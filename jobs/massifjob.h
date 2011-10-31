/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@buffout.org>

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

#ifndef MASSIFJOB_H
# define MASSIFJOB_H

# include "job.h"

namespace valgrind
{

    class MassifJob : public Job
    {

    public:
      MassifJob(KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent = 0);
      virtual ~MassifJob();

    protected:

      virtual void processStarted();
      virtual void processEnded();
      virtual void addToolArgs(QStringList &args, KConfigGroup &cfg) const;


    private:
      QFile	*m_file;

    };

}


#endif //!MASSIFJOB_H
