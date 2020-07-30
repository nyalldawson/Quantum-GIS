/***************************************************************************
  qgsunlittexturematerialwidget.cpp
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

#include "qgsunlittexturematerialwidget.h"

#include "qgsunlittexturematerialsettings.h"
#include "qgis.h"

QgsUnlitTextureMaterialWidget::QgsUnlitTextureMaterialWidget( QWidget *parent )
  : QgsMaterialSettingsWidget( parent )
{
  setupUi( this );

  QgsUnlitTextureMaterialSettings defaultMaterial;
  setSettings( &defaultMaterial, nullptr );
  textureScaleSpinBox->setClearValue( 0 );
  textureRotationSpinBox->setClearValue( 0 );

  connect( textureFile, &QgsImageSourceLineEdit::sourceChanged, this, &QgsUnlitTextureMaterialWidget::changed );
  connect( textureScaleSpinBox, static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ), this, &QgsUnlitTextureMaterialWidget::changed );
  connect( textureRotationSpinBox, static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ), this, &QgsUnlitTextureMaterialWidget::changed );
}

QgsMaterialSettingsWidget *QgsUnlitTextureMaterialWidget::create()
{
  return new QgsUnlitTextureMaterialWidget();
}

void QgsUnlitTextureMaterialWidget::setSettings( const QgsAbstractMaterialSettings *settings, QgsVectorLayer * )
{
  const QgsUnlitTextureMaterialSettings *unlitMaterial = dynamic_cast< const QgsUnlitTextureMaterialSettings * >( settings );
  if ( !unlitMaterial )
    return;

  textureFile->setSource( unlitMaterial->texturePath() );
  textureScaleSpinBox->setValue( unlitMaterial->textureScale() );
  textureRotationSpinBox->setValue( unlitMaterial->textureRotation() );
}

QgsAbstractMaterialSettings *QgsUnlitTextureMaterialWidget::settings()
{
  std::unique_ptr< QgsUnlitTextureMaterialSettings > m = qgis::make_unique< QgsUnlitTextureMaterialSettings >();
  m->setTexturePath( textureFile->source() );
  m->setTextureScale( textureScaleSpinBox->value() );
  m->setTextureRotation( textureRotationSpinBox->value() );
  return m.release();
}
