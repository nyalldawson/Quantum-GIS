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

/**
 * \ingroup core
 * \class QgsMaskEffect
 * \brief A paint effect which masks a source picture, converting some portions
 * of the picture to transparent.
 *
 * \note Added in version 3.12
 */
class CORE_EXPORT QgsMaskEffect : public QgsPaintEffect
{

  public:

    //! Available mask types
    enum MaskType
    {
      SourceIn, //!< Apply the alpha channel from the original source image to the output image
      SourceOut, //!< Apply the inverse of the alpha channel from the original source image to the output image
      FadeFromTop, //!< Fade out from opaque at top to transparent at bottom
      FadeFromBottom, //!< Fade out from opaque at bottom to transparent at top
      FadeFromLeft, //!< Fade out from opaque at left to transparent at right
      FadeFromRight //!< Fade out from opaque at right to transparent at left
    };

    /**
     * Creates a new QgsMaskEffect effect from a properties string \a map.
     *
     * Caller takes ownership of the returned effect.
     */
    static QgsPaintEffect *create( const QgsStringMap & ) SIP_FACTORY;

    QgsMaskEffect() = default;

    virtual QString type() const override;
    virtual QgsStringMap properties() const override;
    virtual void readProperties( const QgsStringMap &props ) override;
    virtual QgsPaintEffect *clone() const override;

    /**
     * Sets the mask \a type, which controls how the opacity of the source
     * image will be masked.
     * \see maskType()
     */
    void setMaskType( const MaskType type );

    /**
     * Returns the mask type, which controls how the opacity of the source
     * image will be masked.
     * \see setMaskType()
     */
    MaskType maskType() const { return mMaskType; }

  protected:

    virtual void draw( QgsRenderContext &context ) override;

  private:

    MaskType mMaskType = SourceIn;

    void drawFade( QPainter &imPainter );
};

#endif // QGSMASKEFFECT_H

