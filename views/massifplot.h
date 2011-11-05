/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#ifndef MASSIFPLOT_H
#define MASSIFPLOT_H

#include <qwt/qwt_plot.h>

class QwtPlotCurve;
class QwtPlotItem;

namespace valgrind
{
    class MassifModel;
    class Model;

    class MassifPlot: public QwtPlot
    {
        Q_OBJECT

    public:
        MassifPlot();
        ~MassifPlot();

        void setModel( Model * );

    protected slots:
        void modelChanged();

    protected:
        MassifModel * m_model;

        QwtPlotCurve * m_memheap;
        QwtPlotCurve * m_memheap_extra;
        QwtPlotCurve * m_memstack;
    };
}

#endif
