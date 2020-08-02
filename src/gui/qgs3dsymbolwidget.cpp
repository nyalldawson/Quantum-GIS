/***************************************************************************
  qgs3dsymbolwidget.cpp
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

#include "qgs3dsymbolwidget.h"
#include "qgsapplication.h"
#include "qgs3dsymbolregistry.h"
#include "qgsabstract3dsymbol.h"
#include "qgsgui.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>

Qgs3DSymbolWidget::Qgs3DSymbolWidget( QWidget *parent )
  : QWidget( parent )
{}

///@cond PRIVATE
QgsAbstract3DSymbolDialogWithPreview::QgsAbstract3DSymbolDialogWithPreview( const QgsAbstract3DSymbol *, QWidget *parent )
  : QDialog( parent )
{

}

QgsAbstract3DSymbol *QgsAbstract3DSymbolDialogWithPreview::symbol() const
{
  return nullptr;
}

QDialogButtonBox *QgsAbstract3DSymbolDialogWithPreview::buttonBox() const
{
  return nullptr;
}

///@endcond
