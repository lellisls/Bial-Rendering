#include "stlviewer.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QOpenGLFunctions>


int rotateX = 0, rotateY = 0, rotateZ = 0; /* rotation amounts about axes, controlled by keyboard */

STLViewer::STLViewer( QWidget *parent ) : QOpenGLWidget( parent ) {
  QStringList args = QApplication::arguments( );
  if( args.size( ) == 2 ) {
    QString fname = args.at( 1 );
    COMMENT( "Loading stl file: " << fname.toStdString( ), 0 );
    mesh = Bial::TriangleMesh::ReadSTLB( fname.trimmed( ).toStdString( ) );
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
      norms = new GLdouble[ mesh->getNverts( ) * 3 ];
      for( size_t t = 0; t < mesh->getNtris( ); ++t ) {
        const Bial::Normal &norm = n[ t ];
        norms[ t * 3 ] = static_cast< GLdouble >( norm.x );
        norms[ t * 3 + 1 ] = static_cast< GLdouble >( norm.y );
        norms[ t * 3 + 2 ] = static_cast< GLdouble >( norm.z );
      }
    }
    else {
      norms = nullptr;
    }
    boundings[ 0 ] = xs;
    boundings[ 1 ] = ys;
    boundings[ 2 ] = zs;

  }
  setFocus();
  setFocusPolicy(Qt::StrongFocus);
}

void STLViewer::initializeGL( ) {
  glClearColor( 0, 0, 0, 1 );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHTING );
  glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
  glEnable( GL_COLOR_MATERIAL );

}

void STLViewer::resizeGL( int w, int h ) {
  glViewport( 0, 0, w, h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 60, ( float ) w / h, .01, 2.0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( 0, 0, -2, 0, 0, 0, 0, 1, 0 );
}


void STLViewer::paintGL( ) {

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity( ); /* Set up modelview transform, first cube. */
  glPushMatrix( );
  glTranslated( 0, 0, -1.5 );

  glScaled( 1.0 / boundings[ 0 ], 1.0 / boundings[ 1 ], 1.0 / boundings[ 2 ] );
  glRotatef( rotateZ, 0, 0, 1 ); /* Apply rotations. */
  glRotatef( rotateY, 0, 1, 0 );
  glRotatef( rotateX, 1, 0, 0 );

  glVertexPointer( 3, GL_DOUBLE, 0, verts );

  glBegin( GL_LINES );
  glColor3f( 1, 0, 0 );
  glVertex3d( 0, 0, 0 );
  glVertex3d( 1, 0, 0 );
  glColor3f( 0, 1, 0 );
  glVertex3d( 0, 0, 0 );
  glVertex3d( 0, 1, 0 );
  glColor3f( 0, 0, 1 );
  glVertex3d( 0, 0, 0 );
  glVertex3d( 0, 0, 1 );
  glEnd( );


  glPushMatrix( );
  glColor3f( 1, 1, 1 );
  glTranslated( -boundings[ 0 ] / 2.0, -boundings[ 1 ] / 2.0, -boundings[ 2 ] / 2.0 );
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnable( GL_POLYGON_OFFSET_FILL );
  glPolygonOffset( 1, 1 );
  glDrawElements( GL_TRIANGLES, mesh->getNtris( ) * 3, GL_UNSIGNED_INT, tris );
  glDisable( GL_POLYGON_OFFSET_FILL );
  glDisableClientState( GL_VERTEX_ARRAY );

  glPopMatrix( );
  glPopMatrix( );
}


void STLViewer::keyPressEvent( QKeyEvent *evt ) {
  switch( evt->key( ) ) {
      case Qt::Key_Left:
      rotateY -= 15;
      break;
      case Qt::Key_Right:
      rotateY += 15;
      break;
      case Qt::Key_Down:
      rotateX -= 15;
      break;
      case Qt::Key_Up:
      rotateX += 15;
      break;
      case Qt::Key_PageUp:
      rotateZ -= 15;
      break;
      case Qt::Key_PageDown:
      rotateZ += 15;
      break;
      case Qt::Key_Home:
      rotateX = rotateY = rotateZ = 0;
      break;
  }
  update( );
}
