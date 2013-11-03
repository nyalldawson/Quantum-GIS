/***************************************************************************
                         qgsoverviewrays.h
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
#ifndef QGSOVERVIEWRAYS_H
#define QGSOVERVIEWRAYS_H

#include "qgscomposeritem.h"
#include "qgsrectangle.h"
#include <QGraphicsRectItem>

class QgsComposition;
class QgsComposerMap;

/** \ingroup MapComposer
 *  \class QgsOverviewRays
 *  \brief Object representing rays for map overviews.
 */

class CORE_EXPORT QgsOverviewRays : public QgsComposerItem
{
    Q_OBJECT

  public:
    /** Constructor. */
    QgsOverviewRays( QgsComposition *composition );
    virtual ~QgsOverviewRays();

    /** return correct graphics item type. Added in v1.7 */
    virtual int type() const { return ComposerOverviewRays; }

    /** \brief Reimplementation of QCanvasItem::paint - draw on canvas */
    void paint( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget );

    /** stores state in Dom element
     * @param elem is Dom element corresponding to 'Composer' tag
     * @param doc write template file
     */
    bool writeXML( QDomElement& elem, QDomDocument & doc ) const;

    /** sets state from Dom document
     * @param itemElem is Dom node corresponding to item tag
     * @param doc is Dom document
     */
    bool readXML( const QDomElement& itemElem, const QDomDocument& doc );
    
    /**Sets the pen to draw rays
    @note this function was added in version 2.1*/
    //void setRayPen( const QPen& p );
    //QPen rayPen() const;

    //QRectF boundingRect() const;
    //void updateBoundingRect();
    
    void setExtentsMap( const QgsComposerMap * map );
    
    void setOverviewRect( QRectF rect );

  public slots:
  
    //called when extents or positions of linked map items changes
    void updateRays();

  private:
    
    const QgsComposerMap* mExtentsMap;
    QRectF mExtentItemRect;
    QRectF mOverviewRect;
    

};

#endif //QGSOVERVIEWRAYS
