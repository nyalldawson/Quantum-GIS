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
  std::unique_ptr< QgsMaskEffect > newEffect = qgis::make_unique< QgsMaskEffect >();
  newEffect->readProperties( map );
  return newEffect.release();
}

QString QgsMaskEffect::type() const
{
  return QString( "mask" );
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

    case FadeFromTop:
    case FadeFromBottom:
    case FadeFromLeft:
    case FadeFromRight:
    {
      drawFade( imPainter );
      break;
    }
  }

  imPainter.end();

  context.painter()->drawImage( imageOffset( context ), image );
}

void QgsMaskEffect::drawFade( QPainter &imPainter )
{
  double width = imPainter.device()->width();
  double height = imPainter.device()->height();

  QGradient gradient;

  // this looks EXTREMELY strange, but is required to workaround a Qt gradient bug.
  // without it, the alpha channel of gradients is not respected
  double qtGradientAlphaBugWorkaround = 0.0000001;

  switch ( mMaskType )
  {
    case FadeFromTop:
      gradient = QLinearGradient( 0, 0, qtGradientAlphaBugWorkaround, height );
      break;
    case FadeFromBottom:
      gradient = QLinearGradient( 0, height, qtGradientAlphaBugWorkaround, 0 );
      break;
    case FadeFromLeft:
      gradient = QLinearGradient( 0, 0, width, qtGradientAlphaBugWorkaround );
      break;
    case FadeFromRight:
      gradient = QLinearGradient( width, 0, 0, qtGradientAlphaBugWorkaround );
      break;
    case SourceIn:
    case SourceOut:
      break;
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
  props.insert( QStringLiteral( "enabled" ), mEnabled ? QStringLiteral( "1" ) : QStringLiteral( "0" ) );
  props.insert( QStringLiteral( "draw_mode" ), QString::number( static_cast< int >( mDrawMode ) ) );
  props.insert( QStringLiteral( "mask_type" ), QString::number( static_cast< int >( mMaskType ) ) );
  return props;
}

void QgsMaskEffect::readProperties( const QgsStringMap &props )
{
  bool ok;
  mEnabled = props.value( QStringLiteral( "enabled" ), QStringLiteral( "1" ) ).toInt();
  mDrawMode = static_cast< QgsPaintEffect::DrawMode >( props.value( QStringLiteral( "draw_mode" ), QStringLiteral( "2" ) ).toInt() );
  QgsMaskEffect::MaskType type = static_cast< QgsMaskEffect::MaskType >( props.value( QStringLiteral( "mask_type" ) ).toInt( &ok ) );
  if ( ok )
  {
    setMaskType( type );
  }
}

QgsPaintEffect *QgsMaskEffect::clone() const
{
  std::unique_ptr< QgsMaskEffect > newEffect = qgis::make_unique< QgsMaskEffect >( *this );
  return newEffect.release();
}

void QgsMaskEffect::setMaskType( const QgsMaskEffect::MaskType type )
{
  mMaskType = type;
  if ( type == SourceIn || type == SourceOut )
  {
    requiresQPainterDpiFix = false;
  }
}
