#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class meshApp : public AppBasic {
  public:
    Timer timer;
    TriMesh mMesh;	
    CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mDistortionX;
	float				mDistortionY;
	float				mDistortionZ;
	float				mCameraDistance;
	params::InterfaceGl	mParams;

    
	void prepareSettings( Settings *settings );
	void reset();
	void setup();
	void createMesh();
    void createParams();
    void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void meshApp::reset(){
    mCenter	= Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f );
	mCameraDistance = 350.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 5000.0f );
    mDistortionX = 0.0f;
    mDistortionY = 0.0f;
    mDistortionZ = 0.0f;
    timer.start();
    createMesh();
}

void meshApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 500 );
	settings->setFrameRate( 60.0f );
}


void meshApp::setup()
{
        
    reset();    
    createParams();
    createMesh();
    
}

void meshApp::createParams(){
    
    mParams = params::InterfaceGl( "Distorting Cube", Vec2i( 200, 210 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=100.0 max=2000.0 step=50.0 keyIncr=s keyDecr=w" );
 	mParams.addParam( "Distortion X", &mDistortionX, "min=-5.0 max=5.0 step=.2.0 keyIncr=+ keyDecr=-" );   
    mParams.addParam( "Distortion Y", &mDistortionY, "min=-5.0 max=5.0 step=.2.0 keyIncr=+ keyDecr=-" );   
    mParams.addParam( "Distortion Z", &mDistortionZ, "min=-5.0 max=5.0 step=.2.0 keyIncr=+ keyDecr=-" );   
	mParams.addSeparator();	
    mParams.addButton("Reset", std::bind( &meshApp::reset, this) );

}

void meshApp::createMesh(){
    // Create the points of our cube
    Vec3f v0(-100,-100,-100);
    Vec3f v1( 100,-100,-100);
    Vec3f v2( 100, 100,-100);
    Vec3f v3(-100, 100,-100);
    Vec3f v4(-100,-100, 100);
    Vec3f v5( 100,-100, 100);
    Vec3f v6( 100, 100, 100);
    Vec3f v7(-100, 100, 100);
    
    // Create the colors for each vertex
    Color c0( 0, 0, 0 );
    Color c1( 1, 0, 0 );
    Color c2( 1, 1, 0 );
    Color c3( 0, 1, 0 );
    Color c4( 0, 0, 1 );
    Color c5( 1, 0, 1 );
    Color c6( 1, 1, 1 );
    Color c7( 0, 1, 1 );
    
    Vec3f faces[6][4] = {  /* Vertices for the 6 faces of a cube. */
        {v0, v1, v2, v3}, {v3, v2, v6, v7}, {v7, v6, v5, v4},
        {v4, v5, v1, v0}, {v5, v6, v2, v1}, {v7, v4, v0, v3} };
    
    Color colors[6][4] = {  /* colors for each vertex of the cube. */
        {c0, c1, c2, c3}, {c3, c2, c6, c7}, {c7, c6, c5, c4},
        {c4, c5, c1, c0}, {c5, c6, c2, c1}, {c7, c4, c0, c3} };
    
    mMesh.clear();
    
    for (int i = 0; i < 6; i++)
    {
        
        mMesh.appendVertex(faces[i][0]);
        mMesh.appendColorRGB(colors[i][0]);
        mMesh.appendVertex(faces[i][1]);
        mMesh.appendColorRGB(colors[i][1]);
        mMesh.appendVertex(faces[i][2]);
        mMesh.appendColorRGB(colors[i][2]);
        mMesh.appendVertex(faces[i][3]);
        mMesh.appendColorRGB(colors[i][3]);
        
        int numberVertices = mMesh.getNumVertices();
        
        mMesh.appendTriangle( numberVertices - 4, numberVertices - 3, numberVertices - 2 );
        mMesh.appendTriangle( numberVertices - 4, numberVertices - 2, numberVertices - 1 );
        
    }

}

void meshApp::mouseDown( MouseEvent event )
{
}

void meshApp::update()
{
    
    if( mMesh.getNumVertices() == 0 ) return;
  
    // update camera information
	mEye	= Vec3f( 0.0f, 0.0f, mCameraDistance );
    mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
    
    
    // store all the mesh information
    
    std::vector<Color> col = mMesh.getColorsRGB();
    std::vector<ci::Vec3f> vec = mMesh.getVertices();
    
    int i, j;
    i = mMesh.getNumVertices();
    j = 0;
    
    mMesh.clear();
    
    // something to add a little movement
    float incX = sin( timer.getSeconds() ) * mDistortionX;
    float incY = sin( timer.getSeconds() ) * mDistortionY;
    float incZ = sin( timer.getSeconds() ) * mDistortionZ;
    
    while(j < i)
    {
        // alter the positions array to get a little dynamism
        vec.at(j).x -= incX;
        vec.at(j+1).x += incX;
        vec.at(j+2).x += incX*2.0f;
        vec.at(j+3).x -= incX*2.0f;
        
        vec.at(j).y -= incY;
        vec.at(j+1).y += incY;
        vec.at(j+2).y += incY*2.0f;
        vec.at(j+3).y -= incY*2.0f;

        vec.at(j).z -= incZ;
        vec.at(j+1).z += incZ;
        vec.at(j+2).z += incZ*2.0f;
        vec.at(j+3).z -= incZ*2.0f;
        
        // now replace it in the mesh
        mMesh.appendVertex( vec.at(j));
        mMesh.appendColorRGB( col.at(j) );
        mMesh.appendVertex( vec.at(j+1));
        mMesh.appendColorRGB( col.at(j+1) );
        mMesh.appendVertex( vec.at(j+2));
        mMesh.appendColorRGB( col.at(j+2) );
        mMesh.appendVertex( vec.at(j+3));
        mMesh.appendColorRGB( col.at(j+3) );
        
        int vIdx0 = mMesh.getNumVertices() - 4;
        int vIdx1 = mMesh.getNumVertices() - 3;
        int vIdx2 = mMesh.getNumVertices() - 2;
        int vIdx3 = mMesh.getNumVertices() - 1;
        
        mMesh.appendTriangle( vIdx0, vIdx1, vIdx2 );
        mMesh.appendTriangle( vIdx0, vIdx2, vIdx3 );
        
        // go to the next triangle pair
        j+=4;
    }
}

void meshApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
 	gl::enableDepthRead();
	gl::enableDepthWrite();
    
    gl::draw(mMesh);
    
	// DRAW PARAMS WINDOW
	params::InterfaceGl::draw();
}


CINDER_APP_BASIC( meshApp, RendererGl )
