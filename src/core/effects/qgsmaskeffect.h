/***************************************************************************
                             qgsmaskeffect.h
                             ---------------
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
#ifndef QGSMASKEFFECT_H
#define QGSMASKEFFECT_H

#include "qgspainteffect.h"
#include "qgis.h"
#include <QPainter>

/** \ingroup core
 * \class QgsMaskEffect
 * \brief A paint effect which masks a source picture, converting some portions
 * of the picture to transparent.
 *
 * \note Added in version 2.9
 */

class CORE_EXPORT QgsMaskEffect : public QgsPaintEffect
{

  public:

    /** Available mask types */
    enum MaskType
    {
      SourceIn, /*< stack blur, a fast but low quality blur. Valid blur level values are between 0 - 16.*/
      SourceOut, /*< Gaussian blur, a slower but high quality blur. Blur level values are the distance in pixels for the blur operation. */
      FadeFromTop,
      FadeFromBottom,
      FadeFromLeft,
      FadeFromRight
    };

    /** Creates a new QgsMaskEffect effect from a properties string map.
     * @param map encoded properties string map
     * @returns new QgsMaskEffect
     */
    static QgsPaintEffect* create( const QgsStringMap& );

    QgsMaskEffect();
    virtual ~QgsMaskEffect();

    virtual QString type() const override { return QString( "mask" ); }
    virtual QgsStringMap properties() const override;
    virtual void readProperties( const QgsStringMap& props ) override;
    virtual QgsPaintEffect* clone() const override;

    /** Sets the blur method (algorithm) to use for performing the blur.
     * @param method blur method
     * @see blurMethod
     */
    void setMaskType( const MaskType type );

    /** Returns the blur method (algorithm) used for performing the blur.
     * @returns blur method
     * @see setBlurMethod
     */
    MaskType maskType() const { return mMaskType; }

  protected:

    virtual void draw( QgsRenderContext& context ) override;


  private:

    MaskType mMaskType;

    void drawFade( QPainter &imPainter );
};

#endif // QGSMASKEFFECT_H

