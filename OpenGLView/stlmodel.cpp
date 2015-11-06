#include "stlmodel.h"

#include <QDebug>
#include <QTime>

StlModel::StlModel( Bial::TriangleMesh *mesh ) : mesh( mesh ) {
  QTime t;
  t.start( );

  mesh = nullptr;
  verts = nullptr;
  norms = nullptr;
  tris = nullptr;
  /*    mesh->Print( std::cout ); */
  size_t *vertexIndex = mesh->getVertexIndex( );
  Bial::Point3D *p = mesh->getP( );
  Bial::Normal *n = mesh->getN( );
  verts = new GLdouble[ mesh->getNverts( ) * 3 ];
  tris = new GLuint[ mesh->getNtris( ) * 3 ];
  for( size_t vtx = 0; vtx < ( mesh->getNtris( ) * 3 ); ++vtx ) {
    tris[ vtx ] = static_cast< GLuint >( vertexIndex[ vtx ] );
  }
  double xs( 0.0 ), ys( 0.0 ), zs( 0.0 );
  for( size_t pt = 0; pt < mesh->getNverts( ); ++pt ) {
    const Bial::Point3D &point = p[ pt ];
    verts[ pt * 3 ] = static_cast< GLdouble >( point.x );
    verts[ pt * 3 + 1 ] = static_cast< GLdouble >( point.y );
    verts[ pt * 3 + 2 ] = static_cast< GLdouble >( point.z );

    xs = std::max( xs, point.x );
    ys = std::max( ys, point.y );
    zs = std::max( zs, point.z );
  }
  if( n != nullptr ) {
    COMMENT( "Reading normals.", 0 );
    norms = new GLdouble[ mesh->getNverts( ) * 3 ];
    for( size_t t = 0; t < mesh->getNverts( ); ++t ) {
      const Bial::Normal &norm = n[ t ];
      norms[ t * 3 ] = 0.0 - static_cast< GLdouble >( norm.x );
      norms[ t * 3 + 1 ] = 0.0 - static_cast< GLdouble >( norm.y );
      norms[ t * 3 + 2 ] = 0.0 - static_cast< GLdouble >( norm.z );
      /*      std::cout << norm << std::endl; */
    }
  }
  else {
    norms = nullptr;
  }
  boundings[ 0 ] = xs;
  boundings[ 1 ] = ys;
  boundings[ 2 ] = zs;
  qDebug( ) << "Elapsed: " << t.elapsed( ) << " ms";
}

StlModel::~StlModel( ) {
  if( mesh ) {
    delete mesh;
  }
  if( verts ) {
    delete[] verts;
  }
  if( norms ) {
    delete[] norms;
  }
  if( tris ) {
    delete[] tris;
  }
}

void StlModel::reload( ) {
  if( verts ) {
    qDebug( ) << "Verts found";
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_DOUBLE, 0, verts );
    glDisableClientState( GL_VERTEX_ARRAY );
  }
  if( norms ) {
    qDebug( ) << "Norms found";
    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_DOUBLE, 0, norms );
    glDisableClientState( GL_NORMAL_ARRAY );
  }
}

void StlModel::draw( ) {
  glPushMatrix( );
  glScaled( 1.0 / boundings[ 0 ], 1.0 / boundings[ 1 ], 1.0 / boundings[ 2 ] );
  GLfloat diffuseCoeff[] = { 0.2f, 0.4f, 0.9f, 1.0f };
  GLfloat specularCoeff[] = { 0.2f, 0.4f, 0.9f, 1.0f };
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuseCoeff );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularCoeff );
  glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 25.0 );
  glColor4f( 1, 1, 1, 1 );
  glTranslated( -boundings[ 0 ] / 2.0, -boundings[ 1 ] / 2.0, -boundings[ 2 ] / 2.0 );
  glEnable( GL_NORMALIZE );
  glEnableClientState( GL_NORMAL_ARRAY );
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glPolygonOffset( 1, 1 );
  if( tris ) {
    glAssert( glDrawElements( GL_TRIANGLES, mesh->getNtris( ) * 3, GL_UNSIGNED_INT, tris ) );
/*    drawNormals( ); */
  }
  glDisable( GL_POLYGON_OFFSET_FILL );
  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_NORMAL_ARRAY );
  glPopMatrix( );
}

void StlModel::drawNormals( ) {
  if( norms ) {
    glBegin( GL_LINES );
    glColor3f( 1, 0, 0 );
    for( size_t i = 0; i < mesh->getNverts( ); ++i ) {
      int x1 = verts[ i * 3 ];
      int y1 = verts[ i * 3 + 1 ];
      int z1 = verts[ i * 3 + 2 ];
      int x2 = x1 + norms[ i * 3 ];
      int y2 = y1 + norms[ i * 3 + 1 ];
      int z2 = z1 + norms[ i * 3 + 2 ];
      glVertex3f( x1, y1, z1 );
      glVertex3f( x2, y2, z2 );

    }
    glEnd( );
  }
}

StlModel* StlModel::loadStl( QString fileName ) {
  COMMENT( "Loading stl file: " << fileName.toStdString( ), 0 );
  return( new StlModel( Bial::TriangleMesh::ReadSTLB( fileName.trimmed( ).toStdString( ) ) ) );

}

StlModel* StlModel::marchingCubes( QString fileName, float isolevel, float scale ) {
  qDebug( ) << "Running marching cubes algorithm.";
  Bial::Image< int > img = Bial::File::Read< int >( fileName.trimmed( ).toStdString( ) );
  if( scale != 1.0 ) {
    img = Bial::Geometrics::Scale( img, scale, true );
  }
  Bial::TriangleMesh *mesh = Bial::MarchingCubes::exec( img, isolevel );
  if( !mesh ) {
    return( nullptr );
  }
  return( new StlModel( mesh ) );

}
