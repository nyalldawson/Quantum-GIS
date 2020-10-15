/***************************************************************************
                         qgsalgorithmexplode.cpp
                         ---------------------
    begin                : April 2018
    copyright            : (C) 2018 by Nyall Dawson
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

#include "qgsalgorithmexplode.h"
#include "qgscurve.h"
#include "qgslinestring.h"
#include "qgscircularstring.h"
#include "qgscompoundcurve.h"
#include "qgsgeometrycollection.h"
#include "qgsgeometryutils.h"

///@cond PRIVATE

QString QgsExplodeAlgorithm::name() const
{
  return QStringLiteral( "explodelines" );
}

QString QgsExplodeAlgorithm::displayName() const
{
  return QObject::tr( "Explode lines" );
}

QStringList QgsExplodeAlgorithm::tags() const
{
  return QObject::tr( "segments,parts" ).split( ',' );
}

QString QgsExplodeAlgorithm::group() const
{
  return QObject::tr( "Vector geometry" );
}

QString QgsExplodeAlgorithm::groupId() const
{
  return QStringLiteral( "vectorgeometry" );
}

QString QgsExplodeAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm takes a lines layer and creates a new one in which each line is replaced by a set of "
                      "lines representing the segments in the original line. Each line in the resulting layer contains only a "
                      "start and an end point, with no intermediate nodes between them.\n\n"
                      "If the input layer consists of CircularStrings or CompoundCurves, the output layer will be of the "
                      "same type and contain only single curve segments." );
}

QList<int> QgsExplodeAlgorithm::inputLayerTypes() const
{
  return QList<int>() << QgsProcessing::TypeVectorLine;
}

QgsProcessing::SourceType QgsExplodeAlgorithm::outputLayerType() const
{
  return QgsProcessing::TypeVectorLine;
}

QgsExplodeAlgorithm *QgsExplodeAlgorithm::createInstance() const
{
  return new QgsExplodeAlgorithm();
}

QString QgsExplodeAlgorithm::outputName() const
{
  return QObject::tr( "Exploded" );
}

QgsWkbTypes::Type QgsExplodeAlgorithm::outputWkbType( QgsWkbTypes::Type inputWkbType ) const
{
  return QgsWkbTypes::singleType( inputWkbType );
}

QgsFeatureList QgsExplodeAlgorithm::processFeature( const QgsFeature &f, QgsProcessingContext &, QgsProcessingFeedback * )
{
  if ( !f.hasGeometry() )
  {
    return QgsFeatureList() << f;
  }
  else
  {
    const QVector<QgsCurve *> parts = QgsGeometryUtils::explodeLineToSegments( f.geometry().constGet() );
    QgsFeature outputFeature;
    QgsFeatureList features;
    features.reserve( parts.size() );
    for ( QgsCurve *part : parts )
    {
      outputFeature.setAttributes( f.attributes() );
      // takes ownership of part!
      outputFeature.setGeometry( QgsGeometry( part ) );
      features << outputFeature;
    }
    return features;
  }
}

QgsFeatureSink::SinkFlags QgsExplodeAlgorithm::sinkFlags() const
{
  return QgsFeatureSink::RegeneratePrimaryKey;
}

///@endcond



