/***************************************************************************
  qgsgraphanalyzer.cpp
  --------------------------------------
  Date                 : 2011-04-14
  Copyright            : (C) 2010 by Yakushev Sergey
  Email                : YakushevS <at> list.ru
****************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/

#include <limits>

#include <QMap>
#include <QVector>
#include <QPair>

#include "qgsgraph.h"
#include "qgsgraphanalyzer.h"

void QgsGraphAnalyzer::dijkstra( const QgsGraph *source, int startPointIdx, int criterionNum, QVector<int> *resultTree, QVector<double> *resultCost )
{
  if ( startPointIdx < 0 || startPointIdx >= source->vertexCount() )
  {
    // invalid start point
    return;
  }

  QVector< double > *result = nullptr;
  if ( resultCost )
  {
    result = resultCost;
  }
  else
  {
    result = new QVector<double>();
  }

  result->clear();
  result->insert( result->begin(), source->vertexCount(), std::numeric_limits<double>::infinity() );
  ( *result )[ startPointIdx ] = 0.0;

  if ( resultTree )
  {
    resultTree->clear();
    resultTree->insert( resultTree->begin(), source->vertexCount(), -1 );
  }

  // used as a priority queue where the keys are the cost and values are the vertex ID
  QMultiMap< double, int > vertexQueue;
  vertexQueue.insert( 0.0, startPointIdx );

  while ( !vertexQueue.empty() )
  {
    QMultiMap< double, int >::iterator it = vertexQueue.begin();
    double curCost = it.key();
    int curVertex = it.value();
    vertexQueue.erase( it );

    // edge index list
    const QgsGraphEdgeIds &outgoingEdges = source->vertex( curVertex ).outgoingEdges();
    for ( int edgeId : outgoingEdges )
    {
      const QgsGraphEdge &arc = source->edge( edgeId );
      double cost = arc.cost( criterionNum ).toDouble() + curCost;

      if ( cost < ( *result ).at( arc.toVertex() ) )
      {
        ( *result )[ arc.toVertex()] = cost;
        if ( resultTree )
        {
          ( *resultTree )[ arc.toVertex()] = edgeId;
        }
        vertexQueue.insert( cost, arc.toVertex() );
      }
    }
  }
  if ( !resultCost )
  {
    delete result;
  }
}

QgsGraph *QgsGraphAnalyzer::shortestTree( const QgsGraph *source, int startVertexIdx, int criterionNum )
{
  QgsGraph *treeResult = new QgsGraph();
  QVector<int> tree;

  QgsGraphAnalyzer::dijkstra( source, startVertexIdx, criterionNum, &tree );

  // sourceVertexIdx2resultVertexIdx
  QVector<int> source2result( tree.size(), -1 );

  // Add reachable vertices to the result
  source2result[ startVertexIdx ] = treeResult->addVertex( source->vertex( startVertexIdx ).point() );
  int i = 0;
  for ( i = 0; i < source->vertexCount(); ++i )
  {
    if ( tree[ i ] != -1 )
    {
      source2result[ i ] = treeResult->addVertex( source->vertex( i ).point() );
    }
  }

  // Add arcs to the result
  for ( i = 0; i < source->vertexCount(); ++i )
  {
    if ( tree[ i ] != -1 )
    {
      const QgsGraphEdge &arc = source->edge( tree[i] );

      treeResult->addEdge( source2result[ arc.fromVertex()], source2result[ arc.toVertex()],
                           arc.strategies() );
    }
  }

  return treeResult;
}
