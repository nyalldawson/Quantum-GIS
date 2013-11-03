/***************************************************************************
                         qgsoverviewrays.cpp
                             -------------------
    begin                : November 2013
    copyright            : (C) 2013 by Nyall Dawson
    email                : nyall.dawson@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsoverviewrays.h"
#include "qgscomposermap.h"

#include <QGraphicsScene>
#include <QPainter>

QgsOverviewRays::QgsOverviewRays( QgsComposition* composition ):
  QgsComposerItem( composition ),
  mExtentsMap( 0 )
{
  setCosmetic( true ); 
  setPen( QColor(0, 0, 0, 200 ) );
}

QgsOverviewRays::~QgsOverviewRays()
{

}

void QgsOverviewRays::paint( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget )
{
  Q_UNUSED( itemStyle );
  Q_UNUSED( pWidget );
  if ( !painter || !mExtentsMap )
  {
    return;
  }

  painter->setRenderHint( QPainter::Antialiasing, true );
  painter->setPen( pen() );
  
  bool showTopLeft = ( mOverviewRect.left() >= mExtentItemRect.left() ) != ( mOverviewRect.top() >= mExtentItemRect.top() );
  bool showTopRight = ( mOverviewRect.right() >= mExtentItemRect.right() ) == ( mOverviewRect.top() >= mExtentItemRect.top() );
  bool showBottomLeft = ( mOverviewRect.left() >= mExtentItemRect.left() ) == ( mOverviewRect.bottom() >= mExtentItemRect.bottom() );
  bool showBottomRight = ( mOverviewRect.right() < mExtentItemRect.right() ) != ( mOverviewRect.bottom() < mExtentItemRect.bottom() );
  
  QRectF itemExtentRect = mExtentItemRect.translated( -transform().dx(), -transform().dy() );
  QRectF itemOverviewRect = mOverviewRect.translated( -transform().dx(), -transform().dy() );

  //top left corners
  if ( showTopLeft )
    painter->drawLine( QPointF( itemExtentRect.left(), itemExtentRect.top() ), QPointF( itemOverviewRect.left(), itemOverviewRect.top() ) );
    
  //top right corners
  if ( showTopRight )
    painter->drawLine( QPointF( itemExtentRect.right(), itemExtentRect.top() ), QPointF( itemOverviewRect.right(), itemOverviewRect.top() ) );    

  //bottom left corners
  if ( showBottomLeft )
    painter->drawLine( QPointF( itemExtentRect.left(), itemExtentRect.bottom() ), QPointF( itemOverviewRect.left(), itemOverviewRect.bottom() ) );

  //bottom right corners
  if ( showBottomRight )
    painter->drawLine( QPointF( itemExtentRect.right(), itemExtentRect.bottom() ), QPointF( itemOverviewRect.right(), itemOverviewRect.bottom() ) );
    
  painter->setBrush( QColor( 0, 255, 0, 20 ) );
  painter->setPen( Qt::NoPen );
  
  //draw shaded region
  QPolygonF shadedRegion;
  //generate a polygon which is bounded by both the overview rect, the rays, and the extent map
  if ( showTopLeft && showBottomRight )
  {
    if ( itemExtentRect.top() < itemOverviewRect.top() )
    {
      shadedRegion << itemOverviewRect.topLeft() << itemExtentRect.topLeft() << itemExtentRect.bottomLeft() << itemExtentRect.bottomRight() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight() << itemOverviewRect.topLeft() ;
    }
    else
    {
      shadedRegion << itemOverviewRect.topLeft() << itemExtentRect.topLeft() << itemExtentRect.topRight() << itemExtentRect.bottomRight() << itemOverviewRect.bottomRight() << itemOverviewRect.bottomLeft() << itemOverviewRect.topLeft() ;    
    }
  }
  else if ( showTopRight && showBottomLeft )
  {
    if ( itemExtentRect.top() < itemOverviewRect.top() )
    {  
      shadedRegion << itemOverviewRect.topRight() << itemExtentRect.topRight() << itemExtentRect.bottomRight() << itemExtentRect.bottomLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.topLeft() << itemOverviewRect.topRight();
    }
    else
    {
      shadedRegion << itemOverviewRect.topRight() << itemExtentRect.topRight() << itemExtentRect.topLeft() << itemExtentRect.bottomLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight();    
    }
  }  
  else if ( showTopRight && showBottomRight )
  {
    shadedRegion << itemExtentRect.topRight() << itemOverviewRect.topRight() << itemOverviewRect.topLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.bottomRight() << itemExtentRect.bottomRight() << itemExtentRect.topRight() ;
  }
  else if ( showBottomLeft && showBottomRight )
  {
    shadedRegion << itemExtentRect.bottomLeft() << itemExtentRect.bottomRight() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight() << itemOverviewRect.topLeft() << itemOverviewRect.bottomLeft() << itemExtentRect.bottomLeft() ;
  }
  else if ( showTopLeft && showBottomRight )
  {
    shadedRegion << itemExtentRect.topLeft() << itemExtentRect.bottomLeft() << itemExtentRect.bottomRight() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight() << itemOverviewRect.topLeft() << itemExtentRect.topLeft() ;
  }
  else if ( showBottomLeft && showTopRight )
  {
    shadedRegion << itemExtentRect.bottomLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.topLeft() << itemOverviewRect.topRight() << itemExtentRect.topRight() << itemExtentRect.topLeft() << itemExtentRect.bottomLeft() ;
  }
  else if ( showBottomLeft && showTopLeft )
  {
    shadedRegion << itemExtentRect.topLeft() << itemExtentRect.bottomLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight() << itemOverviewRect.topLeft() << itemExtentRect.topLeft() ;
  }
  else if ( showTopLeft && showTopRight )
  {
    shadedRegion << itemExtentRect.topLeft() << itemOverviewRect.topLeft() << itemOverviewRect.bottomLeft() << itemOverviewRect.bottomRight() << itemOverviewRect.topRight() << itemExtentRect.topRight() << itemExtentRect.topLeft() ;
  }
      
  painter->drawPolygon( shadedRegion );
            
}

void QgsOverviewRays::updateRays()
{
  if ( !mExtentsMap )
  {
    return;
  }
  
  mExtentItemRect = QRectF( mExtentsMap->scenePos().x(), mExtentsMap->scenePos().y(), mExtentsMap->rect().width(), mExtentsMap->rect().height() );
  
  QRectF bounds = mExtentItemRect.united( mOverviewRect );
  
  setSceneRect( bounds );
}

void QgsOverviewRays::setExtentsMap( const QgsComposerMap * map )
{
  mExtentsMap = map;
  updateRays();
}

void QgsOverviewRays::setOverviewRect( QRectF rect )
{
  mOverviewRect = rect;
  updateRays();  
}
    
bool QgsOverviewRays::writeXML( QDomElement& elem, QDomDocument & doc ) const
{
  Q_UNUSED( elem );
  Q_UNUSED( doc );
  
  return true;
}

bool QgsOverviewRays::readXML( const QDomElement& itemElem, const QDomDocument& doc )
{
  Q_UNUSED( itemElem );
  Q_UNUSED( doc );
  
  return true;
}
    
