/***************************************************************************
  qgsunlittexturematerialsettings.cpp
  --------------------------------------
  Date                 : July 2020
  Copyright            : (C) 2020 by Nyall Dawson
  Email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsunlittexturematerialsettings.h"
#include "qgsapplication.h"
#include "qgsimagecache.h"

#include <QDomElement>
#include <Qt3DRender/QPaintedTextureImage>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QGoochMaterial>
#include <QPainter>

QString QgsUnlitTextureMaterialSettings::type() const
{
  return QStringLiteral( "unlittexture" );
}

bool QgsUnlitTextureMaterialSettings::requiresTextureCoordinates() const
{
  return !mTexturePath.isEmpty();
}

bool QgsUnlitTextureMaterialSettings::supportsTechnique( QgsMaterialSettingsRenderingTechnique technique )
{
  switch ( technique )
  {
    case QgsMaterialSettingsRenderingTechnique::Triangles:
      return true;

    case QgsMaterialSettingsRenderingTechnique::Lines:
    case QgsMaterialSettingsRenderingTechnique::InstancedPoints:
    case QgsMaterialSettingsRenderingTechnique::Points:
      return false;
  }
  return false;
}

QgsAbstractMaterialSettings *QgsUnlitTextureMaterialSettings::create()
{
  return new QgsUnlitTextureMaterialSettings();
}

QgsUnlitTextureMaterialSettings *QgsUnlitTextureMaterialSettings::clone() const
{
  return new QgsUnlitTextureMaterialSettings( *this );
}

float QgsUnlitTextureMaterialSettings::textureRotation() const
{
  return mTextureRotation;
}

void QgsUnlitTextureMaterialSettings::readXml( const QDomElement &elem, const QgsReadWriteContext & )
{
  mTexturePath = elem.attribute( QStringLiteral( "diffuse_texture_path" ), QString() );
  mTextureScale = elem.attribute( QStringLiteral( "texture_scale" ), QString( "1.0" ) ).toFloat();
  mTextureRotation = elem.attribute( QStringLiteral( "texture-rotation" ), QString( "0.0" ) ).toFloat();
}

void QgsUnlitTextureMaterialSettings::writeXml( QDomElement &elem, const QgsReadWriteContext & ) const
{
  elem.setAttribute( QStringLiteral( "diffuse_texture_path" ), mTexturePath );
  elem.setAttribute( QStringLiteral( "texture_scale" ), mTextureScale );
  elem.setAttribute( QStringLiteral( "texture-rotation" ), mTextureRotation );
}

///@cond PRIVATE
class QgsQImageTextureImage : public Qt3DRender::QPaintedTextureImage
{
  public:
    QgsQImageTextureImage( const QImage &image, Qt3DCore::QNode *parent = nullptr )
      : Qt3DRender::QPaintedTextureImage( parent )
      , mImage( image )
    {
      setSize( mImage.size() );
    }

    void paint( QPainter *painter ) override
    {
      painter->drawImage( mImage.rect(), mImage, mImage.rect() );
    }

  private:

    QImage mImage;

};
///@endcond
Qt3DRender::QMaterial *QgsUnlitTextureMaterialSettings::toMaterial( QgsMaterialSettingsRenderingTechnique technique, const QgsMaterialContext &context ) const
{
  switch ( technique )
  {
    case QgsMaterialSettingsRenderingTechnique::Triangles:
    {

      bool fitsInCache = false;
      QImage textureSourceImage = QgsApplication::imageCache()->pathAsImage( mTexturePath, QSize(), true, 1.0, fitsInCache );
      ( void )fitsInCache;

      if ( !textureSourceImage.isNull() && !context.isSelected() )
      {
        Qt3DExtras::QTextureMaterial *material = new Qt3DExtras::QTextureMaterial;

        QgsQImageTextureImage *textureImage = new QgsQImageTextureImage( textureSourceImage );
        Qt3DRender::QTexture2D *texture = new Qt3DRender::QTexture2D();
        texture->addTextureImage( textureImage );

        texture->wrapMode()->setX( Qt3DRender::QTextureWrapMode::Repeat );
        texture->wrapMode()->setY( Qt3DRender::QTextureWrapMode::Repeat );
        texture->wrapMode()->setZ( Qt3DRender::QTextureWrapMode::Repeat );

        texture->setSamples( 4 );

        texture->setGenerateMipMaps( true );
        texture->setMagnificationFilter( Qt3DRender::QTexture2D::Linear );
        texture->setMinificationFilter( Qt3DRender::QTexture2D::Linear );

        material->setTexture( texture );

        float rd[9] =
        {
          mTextureScale, 0, 0,
          0, mTextureScale, 0,
          0, 0, mTextureScale,
        };

        material->setTextureTransform( QMatrix3x3( rd ) );
        return material;
      }
      else
      {
        // if no texture set, or feature is selected, we use a default gooch material
        Qt3DExtras::QGoochMaterial *material  = new Qt3DExtras::QGoochMaterial;
        if ( context.isSelected() )
        {
          // update the material with selection colors
          material->setDiffuse( context.selectionColor() );
        }
        return material;
      }
    }

    case QgsMaterialSettingsRenderingTechnique::InstancedPoints:
    case QgsMaterialSettingsRenderingTechnique::Points:
    case QgsMaterialSettingsRenderingTechnique::Lines:
      return nullptr;
  }
  return nullptr;
}

void QgsUnlitTextureMaterialSettings::addParametersToEffect( Qt3DRender::QEffect * ) const
{
}
