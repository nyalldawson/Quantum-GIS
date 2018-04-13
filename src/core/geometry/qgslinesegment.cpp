/***************************************************************************
                         qgslinesegment.cpp
                         -----------------
    begin                : April 2018
    copyright            : (C) 2018 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

#include "qgslinesegment.h"
#include "qgsgeometryutils.h"

bool QgsLineSegment2D::isNull() const
{
  return mStart == mEnd;
}

int QgsLineSegment2D::pointLeftOfLine( const QgsPointXY &point ) const
{
  return QgsGeometryUtils::leftOfLine( point.x(), point.y(), mStart.x(), mStart.y(), mEnd.x(), mEnd.y() );
}

bool QgsLineSegment2D::intersects( const QgsLineSegment2D &other ) const
{
  bool isIntersect = false;
  QgsPoint out;

  QgsGeometryUtils::segmentIntersection( QgsPoint( mStart ), QgsPoint( mEnd ), QgsPoint( other.start() ), QgsPoint( other.end() ), out, isIntersect );
  return isIntersect;
}

bool QgsLineSegment2D::cutBySegment( const QgsLineSegment2D &cutLine, QgsLineSegment2D &part1, QgsLineSegment2D &part2, QgsLineSegment2D &part3, QgsLineSegment2D &part4 ) const
{
  bool isIntersect = false;
  QgsPoint out;

  if ( !QgsGeometryUtils::segmentIntersection( QgsPoint( mStart ), QgsPoint( mEnd ), QgsPoint( cutLine.start() ), QgsPoint( cutLine.end() ), out, isIntersect, 1e-8, true ) )
    return false;

  QgsPointXY outPt( out );
  if ( ( mStart == outPt || mEnd == outPt ) && ( cutLine.start() == outPt || cutLine.end() == outPt ) )
    return false;

  part1 = QgsLineSegment2D( mStart, outPt );
  part2 = QgsLineSegment2D( outPt, mEnd );
  part3 = QgsLineSegment2D( cutLine.start(), outPt );
  part4 = QgsLineSegment2D( outPt, cutLine.end() );

  return true;
}

