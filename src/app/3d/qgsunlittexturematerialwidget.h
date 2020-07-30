/***************************************************************************
  qgsunlittexturematerialwidget.h
  --------------------------------------
  Date                 : July 2020
  Copyright            : (C) 2020 by Nyall Dawson
  Email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSUNLITTEXTUREMATERIALWIDGET_H
#define QGSUNLITTEXTUREMATERIALWIDGET_H

#include "qgsmaterialsettingswidget.h"
#include "qgsabstractmaterialsettings.h"

#include <ui_unlittexturematerialwidget.h>

class QgsUnlitTextureMaterialSettings;


//! Widget for configuration of unlit texture material settings
class QgsUnlitTextureMaterialWidget : public QgsMaterialSettingsWidget, private Ui::UnlitTextureMaterialWidget
{
    Q_OBJECT
  public:
    explicit QgsUnlitTextureMaterialWidget( QWidget *parent = nullptr );

    static QgsMaterialSettingsWidget *create();
    void setSettings( const QgsAbstractMaterialSettings *settings, QgsVectorLayer *layer ) override;
    QgsAbstractMaterialSettings *settings() override;


};

#endif // QGSUNLITTEXTUREMATERIALWIDGET_H
