/***************************************************************************
                          qgspgquerydialog.cpp
                          --------------------
    begin                : June 2015
    copyright            : (C) 2015 by Nyall Dawson
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

#include "qgspgquerydialog.h"
#include "qgsdatasourceuri.h"
#include "qgspostgresconn.h"
#include "qgspostgresconnpool.h"
#include <QMessageBox>
#include <QSettings>
#include <QTextBrowser>
#include <Qsci/qscilexer.h>



QgsPgQueryDialog::QgsPgQueryDialog( const QString& connectionName, QWidget *parent, Qt::WindowFlags fl )
    : QDialog( parent, fl )
    , mConnName( connectionName )
{
  setupUi( this );

  QSettings s;
  restoreGeometry( s.value( "/Windows/QueryPgConnection/geometry" ).toByteArray() );

  mMessagesBrowser->setFont( getMonospaceFont() );

  mSqlEditor->setFocus();
  mSqlEditor->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  initCompleter();

}

QgsPgQueryDialog::~QgsPgQueryDialog()
{
  QSettings s;
  s.setValue( "/Windows/QueryPgConnection/geometry", saveGeometry() );
}

void QgsPgQueryDialog::on_mClearButton_clicked()
{
  mSqlEditor->clear();
  mIdColCombo->clear();
  mGeomColCombo->clear();
  mIdColCombo->setEnabled( false );
  mGeomColCombo->setEnabled( false );
  mSqlEditor->setFocus();
}

void QgsPgQueryDialog::on_mExecuteButton_clicked()
{
  QgsDataSourceURI uri = QgsPostgresConn::connUri( mConnName );
  QgsPostgresConn *conn = QgsPostgresConnPool::instance()->acquireConnection( uri.connectionInfo() );
  if ( !conn )
  {
    //TODO - show error
    QMessageBox::warning( 0, "ok", "ok!" );
    return;
  }

  QgsPostgresResult result;
  QTime timer;
  timer.start();
  result = conn->PQexec( mSqlEditor->text(), true );
  int ms = timer.elapsed();
  if ( result.PQresultStatus() == PGRES_TUPLES_OK )
  {
    mMessagesBrowser->setText( tr( "Total query runtime: %1 ms.\n%2 %3 retrieved." ).arg( ms )
                               .arg( result.PQcmdTuples() )
                               .arg( result.PQcmdTuples() != "1" ? tr( "rows" ) : tr( "row" ) ) );

    mResultsTabWidget->setCurrentIndex( 0 );
  }
  else if ( result.PQresultStatus() == PGRES_COMMAND_OK )
  {
    mMessagesBrowser->setText( tr( "Query returned successfully: %1 %2 affected, %3 ms execution time." ).arg( result.PQcmdTuples() )
                               .arg( result.PQcmdTuples() != "1" ? tr( "rows" ) : tr( "row" ) )
                               .arg( ms ) );
    mResultsTabWidget->setCurrentIndex( 1 );
  }
  else if ( result.PQresultStatus() == PGRES_FATAL_ERROR )
  {
    mMessagesBrowser->setText( result.PQresultErrorMessage() );
    mResultsTabWidget->setCurrentIndex( 1 );
  }
  QgsPostgresConnPool::instance()->releaseConnection( conn );
}

void QgsPgQueryDialog::on_mRetrieveColumnsButton_clicked()
{
  QString query = mSqlEditor->text().trimmed();

  QString prevIdColName = mIdColCombo->currentText();
  QString prevGeomColName = mGeomColCombo->currentText();

  mIdColCombo->clear();
  mGeomColCombo->clear();
  mIdColCombo->setEnabled( false );
  mGeomColCombo->setEnabled( false );

  if ( query.isEmpty() )
  {
    mMessagesBrowser->setText( QString() );
    return;
  }

  QApplication::setOverrideCursor( Qt::WaitCursor );

  // remove a trailing ';' from query if present
  if ( query.endsWith( ';' ) )
  {
    query.chop( 1 );
  }

  //get the column names
  QgsDataSourceURI uri = QgsPostgresConn::connUri( mConnName );
  QgsPostgresConn *conn = QgsPostgresConnPool::instance()->acquireConnection( uri.connectionInfo() );
  if ( !conn )
  {
    //TODO - show error
    QMessageBox::warning( 0, "ok", "ok!" );
    return;
  }

  QString emptyQuery = QString( "SELECT * FROM (%1) _q LIMIT 0" ).arg( query );

  QgsPostgresResult result;
  result = conn->PQexec( emptyQuery, true );
  if ( result.PQresultStatus() == PGRES_FATAL_ERROR )
  {
    mMessagesBrowser->setText( result.PQresultErrorMessage() );
    mResultsTabWidget->setCurrentIndex( 1 );
    QgsPostgresConnPool::instance()->releaseConnection( conn );
    QApplication::restoreOverrideCursor();
    return;
  }

  mIdColCombo->setEnabled( true );
  mGeomColCombo->setEnabled( true );
  mIdColCombo->addItem( tr("<not set>"));
  mGeomColCombo->addItem( tr("<not set>"));

  int defaultKeyCol = -1;
  int defaultGeomCol = -1;
  int prevKeyCol = -1;
  int prevGeomCol = -1;
  QStringList pkColCandidates = QStringList() << "id" << "uid" << "pid";
  QStringList geomColCandidates = QStringList() << "geom" << "geometry" << "the_geom" << "way";

  mMessagesBrowser->setText( tr( "%1 columns fetched for query.").arg( result.PQnfields() ) );
  for ( int i = 0; i < result.PQnfields(); i++ )
  {
    QString colName = result.PQfname( i );
    if ( defaultKeyCol == -1 && pkColCandidates.contains( colName ) )
    {
      defaultKeyCol = i;
    }
    if ( defaultGeomCol == -1 && geomColCandidates.contains( colName ) )
    {
      defaultGeomCol = i;
    }
    if ( colName == prevIdColName )
    {
      prevKeyCol = i;
    }
    if ( colName == prevGeomColName )
    {
      prevGeomCol = i;
    }

    mIdColCombo->addItem( colName );
    mGeomColCombo->addItem( colName );
  }

  //precedence goes to keeping the previous selected columns
  if ( prevKeyCol >= 0 )
    mIdColCombo->setCurrentIndex( prevKeyCol + 1);
  else if ( defaultKeyCol >= 0 )
    mIdColCombo->setCurrentIndex( defaultKeyCol + 1);

  if ( prevGeomCol >= 0 )
    mGeomColCombo->setCurrentIndex( prevGeomCol + 1 );
  else if ( defaultGeomCol >= 0 )
    mGeomColCombo->setCurrentIndex( defaultGeomCol + 1 );

  QgsPostgresConnPool::instance()->releaseConnection( conn );
  QApplication::restoreOverrideCursor();
}

void QgsPgQueryDialog::initCompleter()
{
  QsciAPIs* api = new QsciAPIs( mSqlEditor->lexer() );

  Q_FOREACH ( QString word, mSqlDictionary )
  {
    api->add( word );
  }

  //get list of actual database objects
  QgsDataSourceURI uri = QgsPostgresConn::connUri( mConnName );
  QgsPostgresConn *conn = QgsPostgresConnPool::instance()->acquireConnection( uri.connectionInfo() );
  if ( conn )
  {
    QgsPostgresResult result;

    QString sql = QString( "SELECT nspname FROM pg_namespace WHERE nspname !~ '^pg_' AND nspname != 'information_schema' UNION SELECT relname FROM pg_class WHERE relkind IN ('v', 'r', 'm') UNION SELECT attname FROM pg_attribute WHERE attnum > 0" );
    result = conn->PQexec( sql, true );
    if ( result.PQresultStatus() == PGRES_TUPLES_OK )
    {
      for ( int idx = 0; idx < result.PQntuples(); idx++ )
      {
        api->add( result.PQgetvalue( idx, 0 ) );
      }
    }
    QgsPostgresConnPool::instance()->releaseConnection( conn );
  }

  api->prepare();
  mSqlEditor->lexer()->setAPIs( api );
  mSqlEditor->setAutoCompletionCaseSensitivity( false );
}

QFont QgsPgQueryDialog::getMonospaceFont()
{
  QSettings settings;
  QString loadFont = settings.value( "pythonConsole/fontfamilytextEditor", "Monospace" ).toString();
  int fontSize = settings.value( "pythonConsole/fontsizeEditor", 10 ).toInt();

  QFont font( loadFont );
  font.setFixedPitch( true );
  font.setPointSize( fontSize );
  font.setBold( false );
  return font;
}


const QStringList QgsPgQueryDialog::mSqlDictionary = QStringList()
    // keywords
    << "action"
    << "add"
    << "after"
    << "all"
    << "alter"
    << "analyze"
    << "and"
    << "as"
    << "asc"
    << "before"
    << "begin"
    << "between"
    << "by"
    << "cascade"
    << "case"
    << "cast"
    << "check"
    << "collate"
    << "column"
    << "commit"
    << "constraint"
    << "create"
    << "cross"
    << "current_date"
    << "current_time"
    << "current_timestamp"
    << "default"
    << "deferrable"
    << "deferred"
    << "delete"
    << "desc"
    << "distinct"
    << "drop"
    << "each"
    << "else"
    << "end"
    << "escape"
    << "except"
    << "exists"
    << "for"
    << "foreign"
    << "from"
    << "full"
    << "group"
    << "having"
    << "ignore"
    << "immediate"
    << "in"
    << "initially"
    << "inner"
    << "insert"
    << "intersect"
    << "into"
    << "is"
    << "isnull"
    << "join"
    << "key"
    << "left"
    << "like"
    << "limit"
    << "match"
    << "natural"
    << "no"
    << "not"
    << "notnull"
    << "null"
    << "of"
    << "offset"
    << "on"
    << "or"
    << "order"
    << "outer"
    << "primary"
    << "references"
    << "release"
    << "restrict"
    << "right"
    << "rollback"
    << "row"
    << "savepoint"
    << "select"
    << "set"
    << "table"
    << "temporary"
    << "then"
    << "to"
    << "transaction"
    << "trigger"
    << "union"
    << "unique"
    << "update"
    << "using"
    << "values"
    << "view"
    << "when"
    << "where"
    << "absolute"
    << "admin"
    << "aggregate"
    << "alias"
    << "allocate"
    << "analyse"
    << "any"
    << "are"
    << "array"
    << "asensitive"
    << "assertion"
    << "asymmetric"
    << "at"
    << "atomic"
    << "authorization"
    << "avg"
    << "bigint"
    << "binary"
    << "bit"
    << "bit_length"
    << "blob"
    << "boolean"
    << "both"
    << "breadth"
    << "call"
    << "called"
    << "cardinality"
    << "cascaded"
    << "catalog"
    << "ceil"
    << "ceiling"
    << "char"
    << "character"
    << "character_length"
    << "char_length"
    << "class"
    << "clob"
    << "close"
    << "coalesce"
    << "collation"
    << "collect"
    << "completion"
    << "condition"
    << "connect"
    << "connection"
    << "constraints"
    << "constructor"
    << "continue"
    << "convert"
    << "corr"
    << "corresponding"
    << "count"
    << "covar_pop"
    << "covar_samp"
    << "cube"
    << "cume_dist"
    << "current"
    << "current_default_transform_group"
    << "current_path"
    << "current_role"
    << "current_transform_group_for_type"
    << "current_user"
    << "cursor"
    << "cycle"
    << "data"
    << "date"
    << "day"
    << "deallocate"
    << "dec"
    << "decimal"
    << "declare"
    << "dense_rank"
    << "depth"
    << "deref"
    << "describe"
    << "descriptor"
    << "destroy"
    << "destructor"
    << "deterministic"
    << "diagnostics"
    << "dictionary"
    << "disconnect"
    << "do"
    << "domain"
    << "double"
    << "dynamic"
    << "element"
    << "end-exec"
    << "equals"
    << "every"
    << "exception"
    << "exec"
    << "execute"
    << "exp"
    << "external"
    << "extract"
    << "false"
    << "fetch"
    << "filter"
    << "first"
    << "float"
    << "floor"
    << "found"
    << "free"
    << "freeze"
    << "function"
    << "fusion"
    << "general"
    << "get"
    << "global"
    << "go"
    << "goto"
    << "grant"
    << "grouping"
    << "hold"
    << "host"
    << "hour"
    << "identity"
    << "ilike"
    << "indicator"
    << "initialize"
    << "inout"
    << "input"
    << "insensitive"
    << "int"
    << "integer"
    << "intersection"
    << "interval"
    << "isolation"
    << "iterate"
    << "language"
    << "large"
    << "last"
    << "lateral"
    << "leading"
    << "less"
    << "level"
    << "ln"
    << "local"
    << "localtime"
    << "localtimestamp"
    << "locator"
    << "lower"
    << "map"
    << "max"
    << "member"
    << "merge"
    << "method"
    << "min"
    << "minute"
    << "mod"
    << "modifies"
    << "modify"
    << "module"
    << "month"
    << "multiset"
    << "names"
    << "national"
    << "nchar"
    << "nclob"
    << "new"
    << "next"
    << "none"
    << "normalize"
    << "nullif"
    << "numeric"
    << "object"
    << "octet_length"
    << "off"
    << "old"
    << "only"
    << "open"
    << "operation"
    << "option"
    << "ordinality"
    << "out"
    << "output"
    << "over"
    << "overlaps"
    << "overlay"
    << "pad"
    << "parameter"
    << "parameters"
    << "partial"
    << "partition"
    << "path"
    << "percentile_cont"
    << "percentile_disc"
    << "percent_rank"
    << "placing"
    << "position"
    << "postfix"
    << "power"
    << "precision"
    << "prefix"
    << "preorder"
    << "prepare"
    << "preserve"
    << "prior"
    << "privileges"
    << "procedure"
    << "public"
    << "range"
    << "rank"
    << "read"
    << "reads"
    << "real"
    << "recursive"
    << "ref"
    << "referencing"
    << "regr_avgx"
    << "regr_avgy"
    << "regr_count"
    << "regr_intercept"
    << "regr_r2"
    << "regr_slope"
    << "regr_sxx"
    << "regr_sxy"
    << "regr_syy"
    << "relative"
    << "result"
    << "return"
    << "returning"
    << "returns"
    << "revoke"
    << "role"
    << "rollup"
    << "routine"
    << "rows"
    << "row_number"
    << "schema"
    << "scope"
    << "scroll"
    << "search"
    << "second"
    << "section"
    << "sensitive"
    << "sequence"
    << "session"
    << "session_user"
    << "sets"
    << "similar"
    << "size"
    << "smallint"
    << "some"
    << "space"
    << "specific"
    << "specifictype"
    << "sql"
    << "sqlcode"
    << "sqlerror"
    << "sqlexception"
    << "sqlstate"
    << "sqlwarning"
    << "sqrt"
    << "start"
    << "state"
    << "statement"
    << "static"
    << "stddev_pop"
    << "stddev_samp"
    << "structure"
    << "submultiset"
    << "substring"
    << "sum"
    << "symmetric"
    << "system"
    << "system_user"
    << "tablesample"
    << "terminate"
    << "than"
    << "time"
    << "timestamp"
    << "timezone_hour"
    << "timezone_minute"
    << "trailing"
    << "translate"
    << "translation"
    << "treat"
    << "trim"
    << "true"
    << "uescape"
    << "under"
    << "unknown"
    << "unnest"
    << "upper"
    << "usage"
    << "user"
    << "value"
    << "varchar"
    << "variable"
    << "varying"
    << "var_pop"
    << "var_samp"
    << "verbose"
    << "whenever"
    << "width_bucket"
    << "window"
    << "with"
    << "within"
    << "without"
    << "work"
    << "write"
    << "xml"
    << "xmlagg"
    << "xmlattributes"
    << "xmlbinary"
    << "xmlcomment"
    << "xmlconcat"
    << "xmlelement"
    << "xmlforest"
    << "xmlnamespaces"
    << "xmlparse"
    << "xmlpi"
    << "xmlroot"
    << "xmlserialize"
    << "year"
    << "zone"

    // functions
    << "coalesce"
    << "nullif"
    << "quote"
    << "random"
    << "replace"
    << "soundex"

    // operators
    << " AND "
    << " OR "
    << "||"
    << " < "
    << " <= "
    << " > "
    << " >= "
    << " = "
    << " <> "
    << " IS "
    << " IS NOT "
    << " IN "
    << " LIKE "
    << " GLOB "
    << " MATCH "
    << " REGEXP "

    // math_functions
    << "Abs"
    << "ACos"
    << "ASin"
    << "ATan"
    << "Cos"
    << "Cot"
    << "Degrees"
    << "Exp"
    << "Floor"
    << "Log"
    << "Log2"
    << "Log10"
    << "Pi"
    << "Radians"
    << "Round"
    << "Sign"
    << "Sin"
    << "Sqrt"
    << "StdDev_Pop"
    << "StdDev_Samp"
    << "Tan"
    << "Var_Pop"
    << "Var_Samp"

    // string functions
    << "Length"
    << "Lower"
    << "Upper"
    << "Like"
    << "Trim"
    << "LTrim"
    << "RTrim"
    << "Replace"
    << "Substr"

    //aggregate functions
    << "Max"
    << "Min"
    << "Avg"
    << "Count"
    << "Sum"
    << "Group_Concat"
    << "Total"
    << "Var_Pop"
    << "Var_Samp"
    << "StdDev_Pop"
    << "StdDev_Samp"

    //postgis functions
    << "box2d"
    << "box3d"
    << "box3d_extent"
    << "geometry"
    << "geometry_dump"
    << "geography"
    << "addgeometrycolumn"
    << "dropgeometrycolumn"
    << "dropgeometrytable"
    << "postgis_full_version"
    << "postgis_geos_version"
    << "postgis_libxml_version"
    << "postgis_lib_build_date"
    << "postgis_lib_version"
    << "postgis_proj_version"
    << "postgis_scripts_build_date"
    << "postgis_scripts_installed"
    << "postgis_scripts_released"
    << "postgis_uses_stats"
    << "postgis_version"
    << "populate_geometry_columns"
    << "probe_geometry_columns"
    << "updategeometrysrid"
    << "ST_bdpolyfromtext"
    << "ST_bdmpolyfromtext"
    << "ST_geogfromtext"
    << "ST_geographyfromtext"
    << "ST_geogfromwkb"
    << "ST_geomcollfromtext"
    << "ST_geomfromewkb"
    << "ST_geomfromewkt"
    << "ST_geometryfromtext"
    << "ST_geomfromgml"
    << "ST_geomfromkml"
    << "ST_gmltosql"
    << "ST_geomfromtext"
    << "ST_geomfromwkb"
    << "ST_linefrommultipoint"
    << "ST_linefromtext"
    << "ST_linefromwkb"
    << "ST_linestringfromwkb"
    << "ST_makebox2d"
    << "ST_makebox3d"
    << "ST_MakeLine"
    << "ST_makeenvelope"
    << "ST_MakePolygon"
    << "ST_MakePoint"
    << "ST_MakePointM"
    << "ST_MLinefromtext"
    << "ST_mpointfromtext"
    << "ST_mpolyfromtext"
    << "ST_Point"
    << "ST_pointfromtext"
    << "ST_pointfromwkb"
    << "ST_Polygon"
    << "ST_polygonfromtext"
    << "ST_wkbtosql"
    << "ST_wkttosql"
    << "GeometryType"
    << "ST_Boundary"
    << "ST_coorddim"
    << "ST_Dimension"
    << "ST_EndPoint"
    << "ST_Envelope"
    << "ST_ExteriorRing"
    << "ST_GeometryN"
    << "ST_GeometryType"
    << "ST_InteriorRingN"
    << "ST_isClosed"
    << "ST_isEmpty"
    << "ST_isRing"
    << "ST_isSimple"
    << "ST_isValid"
    << "ST_isValidReason"
    << "ST_M"
    << "ST_NDims"
    << "ST_NPoints"
    << "ST_NRings"
    << "ST_NumGeometries"
    << "ST_NumInteriorrings"
    << "ST_NumInteriorring"
    << "ST_NumPoints"
    << "ST_PointN"
    << "ST_Srid"
    << "ST_StartPoint"
    << "ST_Summary"
    << "ST_X"
    << "ST_Y"
    << "ST_Z"
    << "*ST_zmflag"
    << "ST_AddPoint"
    << "ST_Affine"
    << "ST_Force2D"
    << "ST_Force3D"
    << "ST_Force3dZ"
    << "ST_Force3DM"
    << "ST_Force_4d"
    << "ST_force_collection"
    << "ST_forcerhr"
    << "ST_linemerge"
    << "ST_collectionextract"
    << "ST_Multi"
    << "ST_removepoint"
    << "ST_reverse"
    << "ST_rotate"
    << "ST_rotatex"
    << "ST_rotatey"
    << "ST_rotatez"
    << "ST_scale"
    << "ST_segmentize"
    << "ST_setpoint"
    << "ST_SetSrid"
    << "ST_SnapToGrid"
    << "ST_Transform"
    << "ST_Translate"
    << "ST_transscale"
    << "ST_asbinary"
    << "ST_asewkb"
    << "ST_asewkt"
    << "ST_asgeojson"
    << "ST_asgml"
    << "ST_ashexewkb"
    << "ST_askml"
    << "ST_assvg"
    << "ST_geohash"
    << "ST_Astext"
    << "ST_Area"
    << "ST_Azimuth"
    << "ST_Centroid"
    << "ST_ClosestPoint"
    << "ST_Contains"
    << "ST_ContainsProperly"
    << "ST_Covers"
    << "ST_CoveredBy"
    << "ST_Crosses"
    << "ST_linecrossingdirection"
    << "ST_Cisjoint"
    << "ST_Distance"
    << "ST_hausdorffdistance"
    << "ST_maxdistance"
    << "ST_Distance_Sphere"
    << "ST_Distance_Spheroid"
    << "ST_DFullyWithin"
    << "ST_DWithin"
    << "ST_Equals"
    << "ST_hasarc"
    << "ST_Intersects"
    << "ST_Length"
    << "ST_Length2d"
    << "ST_length3d"
    << "ST_Length_Spheroid"
    << "ST_length2d_spheroid"
    << "ST_length3d_spheroid"
    << "ST_longestline"
    << "ST_orderingequals"
    << "ST_Overlaps"
    << "ST_perimeter"
    << "ST_perimeter2d"
    << "ST_perimeter3d"
    << "ST_PointOnSurface"
    << "ST_Relate"
    << "ST_ShortestLine"
    << "ST_Touches"
    << "ST_Within"
    << "ST_Buffer"
    << "ST_BuildArea"
    << "ST_Collect"
    << "ST_ConvexHull"
    << "ST_curvetoline"
    << "ST_Difference"
    << "ST_Dump"
    << "ST_dumppoints"
    << "ST_dumprings"
    << "ST_Intersection"
    << "ST_linetocurve"
    << "ST_memunion"
    << "ST_minimumboundingcircle"
    << "ST_polygonize"
    << "ST_shift_longitude"
    << "ST_Simplify"
    << "ST_SimplifyPreserveTopology"
    << "ST_SymDifference"
    << "ST_Union"
    << "ST_Line_Interpolate_Point"
    << "ST_Line_Locate_Point"
    << "ST_Line_Substring"
    << "ST_locate_along_measure"
    << "ST_locate_between_measures"
    << "ST_locatebetweenelevations"
    << "ST_addmeasure"
    << "addauth"
    << "checkauth"
    << "disablelongtransactions"
    << "enablelongtransactions"
    << "lockrow"
    << "unlockrows"
    << "ST_accum"
    << "box2d"
    << "box3d"
    << "ST_estimated_extent"
    << "ST_expand"
    << "ST_Extent"
    << "ST_extent3d"
    << "find_srid"
    << "ST_mem_size"
    << "ST_point_inside_circle"
    << "ST_XMax"
    << "ST_XMin"
    << "ST_YMax"
    << "ST_YMin"
    << "ST_ZMax"
    << "ST_ZMin"
    //constants
    << "null"
    << "false"
    << "true";
