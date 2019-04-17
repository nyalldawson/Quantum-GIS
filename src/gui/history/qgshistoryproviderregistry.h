/***************************************************************************
                            qgshistoryproviderregistry.h
                            --------------------------
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
#ifndef QGSHISTORYPROVIDERREGISTRY_H
#define QGSHISTORYPROVIDERREGISTRY_H

#include "qgis_gui.h"
#include "qgis_sip.h"
#include <QObject>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QVector>

class QgsAbstractHistoryProvider;

class GUI_EXPORT QgsHistoryProviderRegistry : public QObject
{
    Q_OBJECT

 public:

    /**
     * Creates a new empty history provider registry.
     *
     * QgsHistoryProviderRegistry is not usually directly created, but rather accessed through
     * QgsGui::historyProviderRegistry().
    */
    QgsHistoryProviderRegistry( QObject *parent = nullptr );

    ~QgsHistoryProviderRegistry() override;

    /**
     * Adds a \a provider to the registry. Ownership of the provider is
     * transferred to the registry.
     * Returns true if the provider was successfully added.
     */
    bool addProvider( QgsAbstractHistoryProvider* provider SIP_TRANSFER );

    enum class Backend
    {
      Local,
        Project,
    };

    struct HistoryEntryOptions
    {
      //! Whether the entry should be stored within the local history database
      bool storeLocally = true;

      //! Whether the entry should be stored inside the current project's history
      bool storeInProject = true;
    };

    /**
     * Adds an \a entry to the history logs.
     * The \a providerId specifies the history provider responsible for this entry.
     * Entry options are specified via the \a options argument.
     */
   void addEntry( const QString& providerId, const QVariant& entry, const HistoryEntryOptions& options );

   struct EntryDetails
   {
     QDateTime timestamp;

     QString providerId;

     QVariant entry;

   };

   QVector< EntryDetails > queryEntries( const QDateTime &start = QDateTime(), const QDateTime& end = QDateTime(),
                                         const QString &providerId = QString(), Backend = Backend::Local );

private:

QMap< QString, QgsAbstractHistoryProvider * > mProviders;
QVector< EntryDetails > mEntries;



};

#endif //QGSHISTORYPROVIDERREGISTRY_H



