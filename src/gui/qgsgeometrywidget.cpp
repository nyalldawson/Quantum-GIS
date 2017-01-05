
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
{
  QHBoxLayout* layout = new QHBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );

  mLineEdit = new QLineEdit( this );
  mLineEdit->setReadOnly( true );
  mLineEdit->setStyleSheet( QStringLiteral( "font-style: italic; color: grey;" ) );
  mLineEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
  int width = mLineEdit->minimumSizeHint().width();
  mLineEdit->setMinimumWidth( width );

  mButton = new QToolButton( this );
  mButton->setFixedSize( 30, 26 );
  mButton->setStyleSheet( QStringLiteral( "QToolButton{ background: none; border: 1px solid rgba(0, 0, 0, 0%);} QToolButton:focus { border: 1px solid palette(highlight); }" ) );
  mButton->setIconSize( QSize( 24, 24 ) );
  mButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  mButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOffsetCurve.svg" ) ) );

  layout->addWidget( mLineEdit );
  layout->addWidget( mButton );

  setFocusProxy( mLineEdit );

  mMenu = new QMenu( this );

  mCopyWktAction = new QAction( mMenu );
  mCopyWktAction->setText( tr( "Copy as WKT" ) );
  connect( mCopyWktAction, &QAction::triggered, this, &QgsGeometryWidget::copyWkt );

  mCopyGeoJsonAction = new QAction( mMenu );
  mCopyGeoJsonAction->setText( tr( "Copy as GeoJSON" ) );
  connect( mCopyGeoJsonAction, &QAction::triggered, this, &QgsGeometryWidget::copyGeoJson );

  mPasteAction = new QAction( mMenu );
  mPasteAction->setText( tr( "Paste geometry" ) );
  connect( mPasteAction, &QAction::triggered, this, &QgsGeometryWidget::pasteTriggered );

  mClearAction = new QAction( mMenu );
  mClearAction->setText( tr( "Clear" ) );
  connect( mClearAction, &QAction::triggered, this, &QgsGeometryWidget::clearGeometry );

  mMenu->addAction( mCopyWktAction );
  mMenu->addAction( mCopyGeoJsonAction );
  mMenu->addSeparator();
  mMenu->addAction( mPasteAction );
  mMenu->addAction( mClearAction );
  connect( mMenu, &QMenu::aboutToShow, this, &QgsGeometryWidget::prepareMenu );

  mButton->setMenu( mMenu );
  mButton->setPopupMode( QToolButton::InstantPopup );

  setGeometryValue( QgsGeometry() );
}

void QgsGeometryWidget::setGeometryValue( const QgsGeometry& geometry )
{
  if ( !typeIsAcceptable( geometry.wkbType() ) )
  {
    return;
  }

  mGeometry = geometry;
  mLineEdit->setText( mGeometry ? QgsWkbTypes::displayString( mGeometry.wkbType() ) : QgsApplication::nullRepresentation() );

  mCopyGeoJsonAction->setEnabled( mGeometry );
  mCopyWktAction->setEnabled( mGeometry );
  mClearAction->setEnabled( mGeometry );
}

QgsGeometry QgsGeometryWidget::geometryValue() const
{
  return mGeometry;
}

void QgsGeometryWidget::setAcceptedTypes( const QList<QgsWkbTypes::Type> &types )
{
  mAcceptedTypes = types;
}

void QgsGeometryWidget::clearGeometry()
{
  if ( !mGeometry )
    return;

  setGeometryValue( QgsGeometry() );
}

void QgsGeometryWidget::copyWkt()
{
  if ( !mGeometry )
    return;

  QApplication::clipboard()->setText( mGeometry.exportToWkt() );
}

void QgsGeometryWidget::copyGeoJson()
{
  if ( !mGeometry )
    return;

  QApplication::clipboard()->setText( mGeometry.exportToGeoJSON() );
}

void QgsGeometryWidget::pasteTriggered()
{
  if ( mPastedGeom )
  {
    setGeometryValue( mPastedGeom );
    mPastedGeom = QgsGeometry();
  }
}

void QgsGeometryWidget::fetchGeomFromClipboard()
{
  mPastedGeom = QgsGeometry();

  QString text = QApplication::clipboard()->text();
  if ( text.isEmpty() )
    return;

  //try reading as a single wkt string
  mPastedGeom = QgsGeometry::fromWkt( text );
  if ( mPastedGeom && typeIsAcceptable( mPastedGeom.wkbType() ) )
  {
    return;
  }
  mPastedGeom = QgsGeometry();

  //try reading as a list
  QStringList lines = text.split( "\n" );
  if ( !lines.isEmpty() )
  {
    foreach ( QString line, lines )
    {
      QgsGeometry geometry = QgsGeometry::fromWkt( line );
      if ( geometry && typeIsAcceptable( geometry.wkbType() ) )
      {
        mPastedGeom = geometry;
        return;
      }
    }
  }
}

bool QgsGeometryWidget::typeIsAcceptable( QgsWkbTypes::Type type ) const
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
  mPasteAction->setEnabled( mPastedGeom );
}
