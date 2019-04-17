/***************************************************************************
                            qgshistoryproviderregistry.cpp
                            -------------------------
    begin                : April 2019
    copyright            : (C) 2019 by Nyall Dawson
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

#include "qgshistoryproviderregistry.h"
#include "history/qgshistoryprovider.h"
QgsHistoryProviderRegistry::QgsHistoryProviderRegistry(QObject *parent)
    : QObject( parent )
{

}

QgsHistoryProviderRegistry::~QgsHistoryProviderRegistry()
{
    qDeleteAll( mProviders );
}

bool QgsHistoryProviderRegistry::addProvider(QgsAbstractHistoryProvider *provider)
{
   if ( mProviders.contains( provider->id() ) )
        return false;

   mProviders.insert( provider->id(), provider );
   return true;
}

void QgsHistoryProviderRegistry::addEntry(const QString &providerId, const QVariant &entry, const QgsHistoryProviderRegistry::HistoryEntryOptions &options)
{
  EntryDetails deets;
  deets.timestamp = QDateTime::currentDateTime();
  deets.entry = entry;
  deets.providerId = providerId;
  mEntries.append( deets );
}

QVector<QgsHistoryProviderRegistry::EntryDetails> QgsHistoryProviderRegistry::queryEntries(const QDateTime &start, const QDateTime &end, const QString &providerId, QgsHistoryProviderRegistry::Backend)
{
  QVector<EntryDetails > results;
  for ( const EntryDetails& deets : mEntries )
  {
    if ( ( !start.isValid() || start <= deets.timestamp )
         && ( !end.isValid() || end >= deets.timestamp )
         && ( providerId.isEmpty() || providerId == deets.providerId ) )
    {
      results << deets;
    }
  }
  return results;
}
