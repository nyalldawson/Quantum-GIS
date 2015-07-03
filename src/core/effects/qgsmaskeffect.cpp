/***************************************************************************
                              qgsmaskeffect.cpp
                              -----------------
    begin                : April 2015
    copyright            : (C) 2015 Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmaskeffect.h"
#include "qgsimageoperation.h"
#include <QPicture>

QgsPaintEffect *QgsMaskEffect::create( const QgsStringMap &map )
{
  QgsMaskEffect* newEffect = new QgsMaskEffect();
  newEffect->readProperties( map );
  return newEffect;
}

QgsMaskEffect::QgsMaskEffect()
    : QgsPaintEffect()
    , mMaskType( SourceIn )
{

}

QgsMaskEffect::~QgsMaskEffect()
{

}

void QgsMaskEffect::draw( QgsRenderContext &context )
{
  if ( !source() || !enabled() || !context.painter() )
    return;

  QImage image = sourceAsImage( context )->copy();

  QPainter imPainter( &image );
  imPainter.setCompositionMode( QPainter::CompositionMode_DestinationIn );

  switch ( mMaskType )
  {
    case SourceIn:
    {
      if ( mOriginalSource )
        imPainter.drawPicture( 0, 0, *mOriginalSource );
      break;
    }
    case SourceOut:
    {
      imPainter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
      if ( mOriginalSource )
        imPainter.drawPicture( 0, 0, *mOriginalSource );
      break;
    }
    break;

    case FadeFromTop:
    case FadeFromBottom:
    case FadeFromLeft:
    case FadeFromRight:
    {
      drawFade( imPainter );
    }
  }

  imPainter.end();

  context.painter()->drawImage( imageOffset( context ), image );
}

void QgsMaskEffect::drawFade( QPainter& imPainter )
{
  double width = imPainter.device()->width();
  double height = imPainter.device()->height();

  QGradient gradient;
  switch ( mMaskType )
  {
    case FadeFromTop:
      gradient = QLinearGradient( 0, 0, 0, height );
      break;
    case FadeFromBottom:
      gradient = QLinearGradient( 0, height, 0, 0 );
      break;
    case FadeFromLeft:
      gradient = QLinearGradient( 0, 0, width, 0 );
      break;
    case FadeFromRight:
      gradient = QLinearGradient( width, 0, 0, 0 );
      break;
    case SourceIn:
    case SourceOut:
    { } //avoid warnings
  }

  gradient.setColorAt( 0.0, Qt::black );
  gradient.setColorAt( 1.0, Qt::transparent );
  QBrush b( gradient );
  imPainter.setBrush( b );
  imPainter.drawRect( 0, 0, imPainter.device()->width(), imPainter.device()->height() );
}

QgsStringMap QgsMaskEffect::properties() const
{
  QgsStringMap props;
  props.insert( "enabled", mEnabled ? "1" : "0" );
  props.insert( "draw_mode", QString::number( int( mDrawMode ) ) );
  props.insert( "mask_type", QString::number(( int )mMaskType ) );
  return props;
}

void QgsMaskEffect::readProperties( const QgsStringMap &props )
{
  bool ok;
  mEnabled = props.value( "enabled", "1" ).toInt();
  mDrawMode = ( QgsPaintEffect::DrawMode )props.value( "draw_mode", "2" ).toInt();
  QgsMaskEffect::MaskType type = ( QgsMaskEffect::MaskType )props.value( "mask_type" ).toInt( &ok );
  if ( ok )
  {
    mMaskType = type;
  }
}

QgsPaintEffect *QgsMaskEffect::clone() const
{
  QgsMaskEffect* newEffect = new QgsMaskEffect( *this );
  return newEffect;
}
