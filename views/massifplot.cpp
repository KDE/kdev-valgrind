/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>

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

#include <QString>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>

#include <kdebug.h>
#include <klocale.h>

#include "massifplot.h"
#include "massifmodel.h"

namespace valgrind
{
    MassifPlot::MassifPlot( void )
    {
        setTitle(i18n("Massif statistics"));
        setCanvasBackground(  Qt::white );
        insertLegend( new QwtLegend(), QwtPlot::BottomLegend );
        setAutoReplot( false );
        canvas()->setBorderRadius( 10 );
        plotLayout()->setAlignCanvasToScales( true );
        setAxisTitle( QwtPlot::xBottom, i18n( "Snapshots" ) );
        setAxisTitle( QwtPlot::yLeft, i18n( "Memory usage [b]" ) );

        m_memheap = new QwtPlotCurve( i18n( "Memory heap" ) );
        m_memheap->setPen( QColor( Qt::red ) );
        m_memheap->attach( this );

        m_memheap_extra = new QwtPlotCurve( i18n( "Memory heap extra" ) );
        m_memheap_extra->setPen( QColor( Qt::blue ) );
        m_memheap_extra->attach( this );

        m_memstack = new QwtPlotCurve( i18n( "Memory stack" ) );
        m_memstack->setPen( QColor( Qt::darkCyan ) );
        m_memstack->attach( this );

        replot();
    }

    void MassifPlot::setModel( Model * model )
    {
        m_model = dynamic_cast<MassifModel *>( model );
        connect( m_model, SIGNAL( modelChanged() ), this,  SLOT( modelChanged() ) );
    }

    void MassifPlot::modelChanged( void )
    {
        int nb_snaps = m_model->rowCount();
        QPolygonF heap_points, heap_extra_points, stack_points;

        setAxisScale( QwtPlot::xBottom, 0.0, nb_snaps );
        for ( int x = 0; x < nb_snaps; ++x ) {
            heap_points << QPointF( x, m_model->getColumnAtSnapshot(x, MassifItem::MemHeapB).toDouble() );
            heap_extra_points << QPointF( x, m_model->getColumnAtSnapshot(x, MassifItem::MemHeapExtraB).toDouble() );
            stack_points << QPointF( x, m_model->getColumnAtSnapshot(x, MassifItem::MemStacksB).toDouble() );
        }
        m_memheap->setSamples(heap_points);
        m_memheap_extra->setSamples(heap_extra_points);
        m_memstack->setSamples(stack_points);

        replot();
    }

    MassifPlot::~MassifPlot( void )
    {
        delete m_memheap;
        delete m_memheap_extra;
        delete m_memstack;
    }
}
