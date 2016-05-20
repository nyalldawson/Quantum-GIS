
/***************************************************************************
   qgsgeometrywidget.cpp
    --------------------------------------
   Date                 : March 2015
   Copyright            : (C) 2015 Nyall Dawson
   Email                : nyall.dawson@gmail.com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "qgsgeometrywidget.h"
#include "qgsapplication.h"
#include "qgsgeometry.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QClipboard>

QgsGeometryWidget::QgsGeometryWidget( QWidget *parent )
    : QWidget( parent )
    , mLineEdit( nullptr )
{
  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );

  mLineEdit = new QLineEdit( this );
  mLineEdit->setReadOnly( true );
  mLineEdit->setStyleSheet( "font-style: italic; color: grey;" );
  mLineEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
  int width = mLineEdit->minimumSizeHint().width();
  mLineEdit->setMinimumWidth( width );

  mButton = new QToolButton( this );
  mButton->setFixedSize( 30, 26 );
  mButton->setStyleSheet( QString( "QToolButton{ background: none; border: 1px solid rgba(0, 0, 0, 0%);} QToolButton:focus { border: 1px solid palette(highlight); }" ) );
  mButton->setIconSize( QSize( 24, 24 ) );
  mButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  mButton->setIcon( QgsApplication::getThemeIcon( "/mActionOffsetCurve.png" ) );

  layout->addWidget( mLineEdit );
  layout->addWidget( mButton );

  setFocusProxy( mLineEdit );

  mMenu = new QMenu( this );

  mCopyWktAction = new QAction( mMenu );
  mCopyWktAction->setText( tr( "Copy as WKT" ) );
  connect( mCopyWktAction, SIGNAL( triggered() ), this, SLOT( copyWkt() ) );

  mCopyGeoJsonAction = new QAction( mMenu );
  mCopyGeoJsonAction->setText( tr( "Copy as GeoJSON" ) );
  connect( mCopyGeoJsonAction, SIGNAL( triggered() ), this, SLOT( copyGeoJson() ) );

  mPasteAction = new QAction( mMenu );
  mPasteAction->setText( tr( "Paste geometry" ) );
  connect( mPasteAction, SIGNAL( triggered() ), this, SLOT( pasteTriggered() ) );

  mClearAction = new QAction( mMenu );
  mClearAction->setText( tr( "Clear" ) );
  connect( mClearAction, SIGNAL( triggered() ), this, SLOT( clearGeometry() ) );

  mMenu->addAction( mCopyWktAction );
  mMenu->addAction( mCopyGeoJsonAction );
  mMenu->addSeparator();
  mMenu->addAction( mPasteAction );
  mMenu->addAction( mClearAction );
  connect( mMenu, SIGNAL( aboutToShow() ), this, SLOT( prepareMenu() ) );

  mButton->setMenu( mMenu );
  mButton->setPopupMode( QToolButton::InstantPopup );

  setGeometryValue( 0 );
}

void QgsGeometryWidget::setGeometryValue( QgsGeometry *geometry )
{
  setGeometryValue( !geometry ? QgsGeometry() : *geometry );
}

void QgsGeometryWidget::setGeometryValue( const QgsGeometry& geometry )
{
  if ( !geometry.isEmpty() && !typeIsAcceptable( geometry.geometry()->wkbType() ) )
  {
    return;
  }

  mGeometry = geometry;
  mLineEdit->setText( !mGeometry.isEmpty() ? QgsWKBTypes::displayString( mGeometry.geometry()->wkbType() )
                      : tr( "Empty geometry" ) );

  mCopyGeoJsonAction->setEnabled( !mGeometry.isEmpty() );
  mCopyWktAction->setEnabled( !mGeometry.isEmpty() );
  mClearAction->setEnabled( !mGeometry.isEmpty() );
}

QgsGeometry QgsGeometryWidget::geometryValue() const
{
  return mGeometry;
}

void QgsGeometryWidget::setAcceptedTypes( const QList<QgsWKBTypes::Type>& types )
{
  mAcceptedTypes = types;
}

QList<QgsWKBTypes::Type> QgsGeometryWidget::acceptedTypes() const
{
  return mAcceptedTypes;
}

void QgsGeometryWidget::clearGeometry()
{
  mGeometry = QgsGeometry();
}

void QgsGeometryWidget::copyWkt()
{
  if ( mGeometry.isEmpty() )
    return;

  QApplication::clipboard()->setText( mGeometry.exportToWkt() );
}

void QgsGeometryWidget::copyGeoJson()
{
  if ( mGeometry.isEmpty() )
    return;

  QApplication::clipboard()->setText( mGeometry.exportToGeoJSON() );
}

void QgsGeometryWidget::pasteTriggered()
{
  if ( mPastedGeom.data() )
  {
    setGeometryValue( mPastedGeom.data() );
    mPastedGeom.reset();
  }
}

void QgsGeometryWidget::fetchGeomFromClipboard()
{
  mPastedGeom.reset();

  QString text = QApplication::clipboard()->text();
  if ( text.isEmpty() )
    return;

  //try reading as a single wkt string
  mPastedGeom.reset( QgsGeometry::fromWkt( text ) );
  if ( mPastedGeom.data() && ( !mPastedGeom->isEmpty() && typeIsAcceptable( mPastedGeom->geometry()->wkbType() ) ) )
  {
    return;
  }
  mPastedGeom.reset();

  //try reading as a list
  QStringList lines = text.split( "\n" );
  if ( !lines.isEmpty() )
  {
    foreach ( QString line, lines )
    {
      QgsGeometry* geometry = QgsGeometry::fromWkt( line );
      if ( geometry && !geometry->isEmpty() && typeIsAcceptable( geometry->geometry()->wkbType() ) )
      {
        mPastedGeom.reset( geometry );
        return;
      }
      delete geometry;
    }
  }
}

bool QgsGeometryWidget::typeIsAcceptable( QgsWKBTypes::Type type ) const
{
  if ( mAcceptedTypes.isEmpty() )
  {
    return true;
  }

  return mAcceptedTypes.contains( type );
}

void QgsGeometryWidget::prepareMenu()
{
  fetchGeomFromClipboard();
  mPasteAction->setEnabled( mPastedGeom.data() );
}
