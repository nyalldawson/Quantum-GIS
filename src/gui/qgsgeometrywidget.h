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
#include <QList>
#include <QWidget>

class QLineEdit;
class QToolButton;
class QMenu;
class QgsGeometry;

class GUI_EXPORT QgsGeometryWidget : public QWidget
{
    Q_OBJECT

  public:

    explicit QgsGeometryWidget( QWidget *parent = 0 );
    ~QgsGeometryWidget();

    void setGeometryValue( QgsGeometry* geometry );

    QgsGeometry* geometryValue() const;

    void setAcceptedTypes( const QList<QGis::WkbType>& types );

  public slots:

    void clearGeometry();
    void copyWkt();
    void copyGeoJson();
    void pasteTriggered();

  private:
    QLineEdit* mLineEdit;
    QToolButton* mButton;
    QMenu* mMenu;
    QAction* mClearAction;
    QAction* mCopyWktAction;
    QAction* mCopyGeoJsonAction;
    QAction* mPasteAction;
    QgsGeometry* mGeometry;
    QgsGeometry* mPastedGeom;
    QList<QGis::WkbType> mAcceptedTypes;

    void fetchGeomFromClipboard();
    bool typeIsAcceptable( QGis::WkbType type ) const;

  private slots:

    void prepareMenu();
};

#endif // QGSGEOMETRYWIDGET_H
