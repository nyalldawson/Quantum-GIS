/***************************************************************************
  qgsunlittexturematerialsettings.h
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

#ifndef QGSUNLITTEXTUREMATERIALSETTINGS_H
#define QGSUNLITTEXTUREMATERIALSETTINGS_H

#include "qgis_3d.h"
#include "qgsabstractmaterialsettings.h"

#include <QColor>

class QDomElement;

/**
 * \ingroup 3d

 *
 * \since QGIS 3.16
 */
class _3D_EXPORT QgsUnlitTextureMaterialSettings : public QgsAbstractMaterialSettings
{
  public:

    /**
     * Constructor for QgsUnlitTextureMaterialSettings.
     */
    QgsUnlitTextureMaterialSettings() = default;

    QString type() const override;
    bool requiresTextureCoordinates() const override;

    /**
     * Returns TRUE if the specified \a technique is suppored by the material.
     */
    static bool supportsTechnique( QgsMaterialSettingsRenderingTechnique technique );

    /**
     * Returns a new instance of QgsUnlitTextureMaterialSettings.
     */
    static QgsAbstractMaterialSettings *create() SIP_FACTORY;

    QgsUnlitTextureMaterialSettings *clone() const override SIP_FACTORY;

    /**
     * Returns the texture path.

     * \see setTexturePath()
     */
    QString texturePath() const { return mTexturePath; }

    /**
     * Returns the texture scale
     * The texture scale changes the size of the displayed texture in the 3D scene
     * If the texture scale is less than 1 the texture will be stretched
     */
    float textureScale() const { return mTextureScale; }

    float textureRotation() const override;

    /**
     * Sets the \a path of the texture.
     *
     * \see texturePath()
     */
    void setTexturePath( const QString &path ) { mTexturePath = path; }

    /**
     * Sets the texture scale
     * The texture scale changes the size of the displayed texture in the 3D scene
     * If the texture scale is less than 1 the texture will be stretched
     */
    void setTextureScale( float scale ) { mTextureScale = scale; }

    //! Sets the texture rotation in degrees
    void setTextureRotation( float rotation ) { mTextureRotation = rotation; }

    void readXml( const QDomElement &elem, const QgsReadWriteContext &context ) override;
    void writeXml( QDomElement &elem, const QgsReadWriteContext &context ) const override;
#ifndef SIP_RUN
    Qt3DRender::QMaterial *toMaterial( QgsMaterialSettingsRenderingTechnique technique, const QgsMaterialContext &context ) const override SIP_FACTORY;
    void addParametersToEffect( Qt3DRender::QEffect *effect ) const override;
#endif

    bool operator==( const QgsUnlitTextureMaterialSettings &other ) const
    {
      return mTexturePath == other.mTexturePath &&
             mTextureScale == other.mTextureScale &&
             mTextureRotation == other.mTextureRotation;
    }

  private:

    QString mTexturePath;
    float mTextureScale{ 1.0f };
    float mTextureRotation{ 0.0f };
};


#endif // QGSPHONGMATERIALSETTINGS_H
