/***************************************************************************
 qgsmarkersymbolbounds.h
 -----------------------
 begin                : May 2021
 copyright            : (C) 2021 by Nyall Dawson
 email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMARKERSYMBOLBOUNDS_H
#define QGSMARKERSYMBOLBOUNDS_H

#include "qgis_core.h"
#include "qgis_sip.h"
#include <QRectF>

/**
 * \ingroup core
 * \class QgsMarkerSymbolBounds
 *
 * \brief Describes the bounds of a marker symbol.
 *
 * \since QGIS 3.20
 */
class CORE_EXPORT QgsMarkerSymbolBounds
{
  public:

    /**
     * Returns TRUE if the bounds are null or unset.
     */
    bool isNull() const { return mBoundingBox.isNull(); }

    /**
     * Returns the bounding box of the symbol, which represents
     * the extent of the rendered symbol.
     *
     * \see setBoundingBox()
     */
    QRectF boundingBox() const { return mBoundingBox; }

    /**
     * Sets the bounding box of the symbol, which represents
     * the extent of the rendered symbol.
     *
     * \see boundingBox()
     */
    void setBoundingBox( const QRectF &bounds ) { mBoundingBox = bounds;}

    /**
     * Unites these bounds with an \a other bounds.
     */
    void unite( const QgsMarkerSymbolBounds &other )
    {
      const double topLeftX1 = mBoundingBox.left() + mBoundingBox.width() * ( 0.5 + mTopLeftOffset.x() * 0.5 );
      const double topLeftX2 = other.mBoundingBox.left() + other.mBoundingBox.width() * ( 0.5 + other.mTopLeftOffset.x() * 0.5 );
      const double topLeftY1 = mBoundingBox.top() - mBoundingBox.height() * ( 0.5 - mTopLeftOffset.y() * 0.5 );
      const double topLeftY2 = other.mBoundingBox.top() - other.mBoundingBox.height() * ( 0.5 - other.mTopLeftOffset.y() * 0.5 );

      mBoundingBox = mBoundingBox.united( other.boundingBox() );

      mTopLeftOffset.setX( ( mBoundingBox.center().x() - std::min( topLeftX1, topLeftX2 ) ) / ( mBoundingBox.width() * 0.5 ) );
      mTopLeftOffset.setY( ( mBoundingBox.center().y() - std::min( topLeftY1, topLeftY2 ) ) / ( mBoundingBox.height() * 0.5 ) );

      mTopRightOffset.setX( std::max( mTopLeftOffset.x(), other.mTopLeftOffset.x() ) );
      mTopRightOffset.setY( std::min( mTopLeftOffset.y(), other.mTopLeftOffset.y() ) );

      mBottomLeftOffset.setX( std::min( mBottomLeftOffset.x(), other.mBottomLeftOffset.x() ) );
      mBottomLeftOffset.setY( std::max( mBottomLeftOffset.y(), other.mBottomLeftOffset.y() ) );

      mBottomRightOffset.setX( std::max( mBottomRightOffset.x(), other.mBottomRightOffset.x() ) );
      mBottomRightOffset.setY( std::max( mBottomRightOffset.y(), other.mBottomRightOffset.y() ) );
    }

#ifndef SIP_RUN

    /**
     * Returns the corner offset, which is the offset from the center of the symbol to the
     * furthest actual marker content in the specified content.
     *
     * Conceptually this represents the point at which a label placed in that corner should
     * be anchored to in order to be placed directly adjacent to the symbol at that corner.
     *
     * \note Not exposed to Python bindings. This is considered experimental API only.
     *
     * \see setCornerOffset()
     */
    QPointF cornerOffset( Qt::Corner corner ) const
    {
      switch ( corner )
      {
        case Qt::TopLeftCorner:
          return mTopLeftOffset;
        case Qt::TopRightCorner:
          return mTopRightOffset;
        case Qt::BottomLeftCorner:
          return mBottomLeftOffset;
        case Qt::BottomRightCorner:
          return mBottomRightOffset;
      }
      return QPointF();
    }

    /**
     * Sets the \a offset for the specified \a corner, which is the offset from the center of the symbol to the
     * furthest actual marker content in the specified content.
     *
     * Conceptually this represents the point at which a label placed in that corner should
     * be anchored to in order to be placed directly adjacent to the symbol at that corner.
     *
     * \note Not exposed to Python bindings. This is considered experimental API only.
     *
     * \see cornerOffset()
     */
    void setCornerOffset( Qt::Corner corner, QPointF offset )
    {
      switch ( corner )
      {
        case Qt::TopLeftCorner:
          mTopLeftOffset = offset;
          break;
        case Qt::TopRightCorner:
          mTopRightOffset = offset;
          break;
        case Qt::BottomLeftCorner:
          mBottomLeftOffset = offset;
          break;
        case Qt::BottomRightCorner:
          mBottomRightOffset = offset;
          break;
      }
    }
#endif

  private:

    QRectF mBoundingBox;
    QPointF mTopLeftOffset{ -1, 1};
    QPointF mTopRightOffset{ 1, 1};
    QPointF mBottomLeftOffset{ -1, -1};
    QPointF mBottomRightOffset{ 1, -1};

};

#endif // QGSMARKERSYMBOLBOUNDS_H

