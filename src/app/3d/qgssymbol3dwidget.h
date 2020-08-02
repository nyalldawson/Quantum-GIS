/***************************************************************************
  qgssymbol3dwidget.h
  --------------------------------------
  Date                 : January 2019
  Copyright            : (C) 2019 by Martin Dobias
  Email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSSYMBOL3DWIDGET_H
#define QGSSYMBOL3DWIDGET_H

#include "qgs3dsymbolwidget.h"
#include "qgswkbtypes.h"
#include <QWidget>
#include <QDialog>
#include <memory>

class QLabel;
class QStackedWidget;

class QgsAbstract3DSymbol;
class QgsStyleItemsListWidget;

class QgsVectorLayer;
class QDialogButtonBox;
class Qgs3DMapCanvas;

/**
 * Widget for selection of 3D symbol
 */
class QgsSymbol3DWidget : public QWidget
{
    Q_OBJECT

  public:
    QgsSymbol3DWidget( QWidget *parent = nullptr, QgsWkbTypes::GeometryType layerType = QgsWkbTypes::UnknownGeometry, QgsVectorLayer *layer = nullptr );

    //! Returns a new symbol instance or NULLPTR
    std::unique_ptr< QgsAbstract3DSymbol > symbol();

    //! Sets symbol (does not take ownership)
    void setSymbol( const QgsAbstract3DSymbol *symbol, QgsWkbTypes::GeometryType layerType, QgsVectorLayer *vlayer );

  signals:
    void widgetChanged();

  private slots:

    void setSymbolFromStyle( const QString &name );
    void saveSymbol();

  private:

    void updateSymbolWidget( const QgsAbstract3DSymbol *newSymbol );

    QStackedWidget *widgetStack = nullptr;
    QLabel *widgetUnsupported = nullptr;

    QgsStyleItemsListWidget *mStyleWidget = nullptr;

    QgsVectorLayer *mLayer = nullptr;
    QgsWkbTypes::GeometryType mLayerType = QgsWkbTypes::UnknownGeometry;

};


#include "ui_symbol3dwidget.h"

/**
 * Widget for selection of 3D symbol
 */
class QgsSymbolConfig3DWidget : public QWidget, private Ui::QgsSymbol3DConfigWidgetBase
{
    Q_OBJECT

  public:
    QgsSymbolConfig3DWidget( QgsVectorLayer *layer, QWidget *parent = nullptr );

    //! Returns a new symbol instance or NULLPTR
    std::unique_ptr< QgsAbstract3DSymbol > symbol();

    //! Sets symbol (does not take ownership)
    void setSymbol( const QgsAbstract3DSymbol *symbol, QgsWkbTypes::GeometryType layerType, QgsVectorLayer *vlayer );

  private:

    void updatePreview( QgsAbstract3DSymbol *symbol );

    QgsVectorLayer *mLayer = nullptr;
    Qgs3DMapCanvas *mCanvas = nullptr;
    QgsVectorLayer *mTempLayer = nullptr;

};

class Qgs3DSymbolConfigDialog : public QgsAbstract3DSymbolDialogWithPreview
{
    Q_OBJECT

  public:

    /**
     * Constructor for Qgs3DSymbolConfigDialog, initially showing the specified \a symbol.
     */
    Qgs3DSymbolConfigDialog( const QgsAbstract3DSymbol *symbol,
                             QgsWkbTypes::GeometryType layerType,
                             QWidget *parent = nullptr );

    /**
     * Returns a new instance of the symbol defined by the dialog.
     *
     * Caller takes ownership of the returned symbol.
     */
    QgsAbstract3DSymbol *symbol() const override;

    /**
     * Returns a reference to the dialog's button box.
     */
    QDialogButtonBox *buttonBox() const override;

  private:

    QgsSymbolConfig3DWidget *mWidget = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;

};


#endif // QGSSYMBOL3DWIDGET_H
