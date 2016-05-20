/** \ingroup gui
 * \class QgsGeometryWidget
 * \brief A widget for simple manipulation and modification of geometries.
 *
 * \note Added in version 2.16
 */

class QgsGeometryWidget : QWidget
{
%TypeHeaderCode
#include <qgsgeometrywidget.h>
%End

  public:

    /** Constructor for QgsGeometryWidget.
     * @param parent parent widget
     */
    explicit QgsGeometryWidget( QWidget *parent /TransferThis/ = 0 );

    /** Sets the current geometry value for the widget.
     * @param geometry geometry value. Can be null to represent "no geometry" values.
     * Ownership is not transferred.
     * @see geometryValue()
     */
    void setGeometryValue( QgsGeometry* geometry );

    /** Sets the current geometry value for the widget.
     * @param geometry geometry value. Use an empty QgsGeometry() to represent
     * "no geometry" values.
     * @see geometryValue()
     */
    void setGeometryValue( const QgsGeometry& geometry );

    /** Returns a copy of the current geometry value. The returns geometry may be
     * empty (see QgsGeometry::isEmpty() ) if the widget is set to a null geometry value.
     * @see setGeometryValue()
     */
    QgsGeometry geometryValue() const;

    /** Sets the list of geometry types which can be accepted by the widget. This allows
     * the widget to block being set to an incompatible type, ie pasting a linestring
     * in a widget which requires polygons.
     * @param types list of acceptable types, or empty list to accept all types
     * @see acceptedTypes()
     */
    void setAcceptedTypes( const QList< QgsWKBTypes::Type >& types );

    /** Returns the list of geometry types which are accepted by the widget, or an empty
     * list if all types are acceptable.
     * @see setAcceptedType()
     */
    QList< QgsWKBTypes::Type > acceptedTypes() const;

  public slots:

    //! Clears the current geometry by setting it to an empty geometry.
    void clearGeometry();

    //! Sets the clipboard to a WKT representation of the geometry.
    //! @see copyGeoJson()
    void copyWkt();

    //! Sets the clipboard to a GeoJSON representation of the geometry.
    //! @see copyWkt()
    void copyGeoJson();

    //! Try to paste the contents of the clipboard as a geometry.
    void pasteTriggered();

};
