/***************************************************************************
                            qgshistoryprovider.h
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
#ifndef QGSHISTORYPROVIDER_H
#define QGSHISTORYPROVIDER_H

#include "qgis_gui.h"
#include "qgis_sip.h"
#include <QString>
#include <QVariant>
class QgsHistoryEntryNode;

class GUI_EXPORT QgsAbstractHistoryProvider
{
 public:

    /**
     * Returns the provider's unique id, which is used to associate existing history entries with the provider.
     */
    virtual QString id() const = 0;

    /**
     * Creates a new history node for the given \a entry.
     */
    virtual QgsHistoryEntryNode* createNodeForEntry( const QVariant& entry ) = 0 SIP_FACTORY;

};

#endif //QGSHISTORYPROVIDER_H



