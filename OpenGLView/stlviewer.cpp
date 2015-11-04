#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QTime>

#include "MarchingCubes.hpp"
#include "glassert.h"
#include "stlviewer.h"


#include "glassert.h"

STLViewer::STLViewer( QWidget *parent ) : QOpenGLWidget( parent ) {
  setFocus( );
  setFocusPolicy( Qt::StrongFocus );

  QSurfaceFormat format;
  format.setDepthBufferSize( 24 );
  format.setStencilBufferSize( 8 );
  format.setVersion( 3, 2 );
  format.setProfile( QSurfaceFormat::CoreProfile );
  QSurfaceFormat::setDefaultFormat( format );
  setFormat( format );
}

STLViewer::~STLViewer( ) {
  makeCurrent( );

  clear( );

  doneCurrent( );
}

void STLViewer::LoadFile( QString fileName ) {
  clear( );
  resetTransform( );
  model = new StlModel( fileName );
}

GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };

void STLViewer::prepareShaderProgram( ) {
  if( !mShaderProgram.addShaderFromSourceFile( QOpenGLShader::Vertex, ":shaders/phong.vert" ) ) {
    qCritical( ) << "error";
  }
  if( !mShaderProgram.addShaderFromSourceFile( QOpenGLShader::Fragment, ":shaders/phong.frag" ) ) {
    qCritical( ) << "error";
  }
  if( !mShaderProgram.link( ) ) {
    qCritical( ) << "error";
  }
  glCheckError( );
}

void STLViewer::prepareVertexBuffers( ) {
  float positionData[] = {
    -0.8f, -0.8f, 0.0f,
    0.8f, -0.8f, 0.0f,
    0.0f, 0.8f, 0.0f
  };
  float colorData[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };

  mVAO.create( );
  mVAO.bind( );

  mVertexPositionBuffer.create( );
  mVertexPositionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
  mVertexPositionBuffer.bind( );
  mVertexPositionBuffer.allocate( positionData, 3 * 3 * sizeof( float ) );

  mVertexColorBuffer.create( );
  mVertexColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
  mVertexColorBuffer.bind( );
  mVertexColorBuffer.allocate( colorData, 3 * 3 * sizeof( float ) );

  mShaderProgram.bind( );

  mVertexPositionBuffer.bind( );
  mShaderProgram.enableAttributeArray( "vertexPosition" );
  mShaderProgram.setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

  mVertexColorBuffer.bind( );
  mShaderProgram.enableAttributeArray( "vertexColor" );
  mShaderProgram.setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );
  glCheckError( );
}

void STLViewer::initializeGL( ) {
  prepareShaderProgram( );

  prepareVertexBuffers( );

/*
 *  glEnable( GL_LIGHTING );
 *  glClearColor( 0, 0, 0, 1 );
 *  glShadeModel( GL_SMOOTH );
 *  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientLight );
 *  glLightfv( GL_LIGHT0, GL_DIFFUSE, ambientLight );
 *  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
 *  glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
 *  glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 60.0f );
 *  glEnable( GL_LIGHT0 );
 */

/*  glEnable( GL_DEPTH_TEST ); */
}

void STLViewer::resizeGL( int w, int h ) {
  glAssert( glViewport( 0, 0, w, h ) );

//  glViewport( 0, 0, w, h );
//  glMatrixMode( GL_PROJECTION );
//  glLoadIdentity( );
//  gluPerspective( 60, ( float ) w / h, .01, 2.0 );
//  glMatrixMode( GL_MODELVIEW );
//  glLoadIdentity( );
///*  gluLookAt( 0, 0, -2, 0, 0, 0, 0, 1, 0 ); */
//  model->reload( );
//  glCheckError( );
}


void STLViewer::paintGL( ) {

  glAssert( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );

  mShaderProgram.bind( );
  mVAO.bind( );

  glAssert( glDrawArrays( GL_TRIANGLES, 0, 3 ) );


//  /* Cleaning screen */
//  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//  glClearColor( 0, 0, 0, 1 );

//  /* Loading modelView matrix */
//  glMatrixMode( GL_MODELVIEW );
//  glLoadIdentity( );

//  Bial::Transform3D transf;
//  transf.Translate( 0, 0, -1.5 ).Scale( zoom, zoom, zoom );
//  transf.Rotate( rotateX, 0 ).Rotate( rotateY, 1 ).Rotate( rotateZ, 2 );
//  glLoadMatrixd( &transf.getAffineMatrix( ).Transposed( )[ 0 ] );

//  model->draw( );

///*  light1.draw( ); */

///*  drawLines( ); */

//  glCheckError( );
}

void STLViewer::drawLines( ) {
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
}

void STLViewer::clear( ) {
  delete model;
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
      resetTransform( );
      break;
  }
  update( );
}


void STLViewer::mousePressEvent( QMouseEvent *evt ) {
  if( evt->button( ) == Qt::LeftButton ) {
    dragging = true;
    lastPoint = evt->pos( );
  }
  evt->accept( );
}

void STLViewer::mouseReleaseEvent( QMouseEvent *evt ) {
  if( evt->button( ) == Qt::LeftButton ) {
    dragging = false;
    QPoint diff = evt->pos( ) - lastPoint;
    rotateX += diff.y( );
    rotateY += diff.x( );
    lastPoint = evt->pos( );
  }
  evt->accept( );
}

void STLViewer::mouseMoveEvent( QMouseEvent *evt ) {
  if( dragging ) {
    QPoint diff = evt->pos( ) - lastPoint;
    rotateX += diff.y( );
    rotateY += diff.x( );
    lastPoint = evt->pos( );
  }
  evt->accept( );
  update( );
}

void STLViewer::wheelEvent( QWheelEvent *evt ) {
  QPoint numDegrees = evt->angleDelta( ) / 8;
  if( !numDegrees.isNull( ) ) {
    QPoint numSteps = numDegrees / 15;
    zoom += 0.1 * numSteps.ry( );
  }
  zoom = std::max( 1.0, zoom );
  evt->accept( );
  update( );
}

void STLViewer::resetTransform( ) {
  rotateX = rotateY = rotateZ = 0;
  zoom = 1.0;
}

void STLViewer::mouseDoubleClickEvent( QMouseEvent *evt ) {
  resetTransform( );
  evt->accept( );
  update( );
}
