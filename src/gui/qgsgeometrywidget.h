/***************************************************************************
   qgsgeometrywidget.h
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

#ifndef QGSGEOMETRYWIDGET_H
#define QGSGEOMETRYWIDGET_H

#include "qgis.h"
#include "qgsgeometry.h"
#include <QList>
#include <QWidget>

class QLineEdit;
class QToolButton;
class QMenu;

class GUI_EXPORT QgsGeometryWidget : public QWidget
{
    Q_OBJECT

  public:

    explicit QgsGeometryWidget( QWidget *parent = nullptr );

    void setGeometryValue( const QgsGeometry& geometry );

    QgsGeometry geometryValue() const;

    void setAcceptedTypes( const QList<QgsWkbTypes::Type>& types );

  public slots:

    void clearGeometry();
    void copyWkt();
    void copyGeoJson();
    void pasteTriggered();

  private:
    QLineEdit* mLineEdit = nullptr;
    QToolButton* mButton = nullptr;
    QMenu* mMenu = nullptr;
    QAction* mClearAction = nullptr;
    QAction* mCopyWktAction = nullptr;
    QAction* mCopyGeoJsonAction = nullptr;
    QAction* mPasteAction = nullptr;
    QgsGeometry mGeometry;
    QgsGeometry mPastedGeom;
    QList<QgsWkbTypes::Type> mAcceptedTypes;

    void fetchGeomFromClipboard();
    bool typeIsAcceptable( QgsWkbTypes::Type type ) const;

  private slots:

    void prepareMenu();
};

#endif // QGSGEOMETRYWIDGET_H
