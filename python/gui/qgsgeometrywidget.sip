class QgsGeometryWidget : QWidget
{
%TypeHeaderCode
#include <qgsgeometrywidget.h>
%End

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
    
};
