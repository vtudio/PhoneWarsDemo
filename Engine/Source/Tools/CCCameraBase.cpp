/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCCameraBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCAppManager.h"

#ifdef IOS
#include <OpenGLES/ES1/gl.h>
#endif

CCCameraBase *CCCameraBase::currentCamera = NULL;


CCCameraBase::CCCameraBase()
{
    enabled = true;
    index = 0;
    frameBufferID = -1;
    
    updating = true;
    updatedPosition = false;
    updateFOV = false;
    
    aspectRatio = 1.0f;
    perspective = 60.0f;
    zNear = 1.0f;
    zFar = 3000.0f;
	
    CCMatrixLoadIdentity( modelViewMatrix );
    
    // Initialise frustum
    for( uint x=0; x<6; ++x )
    {
        for( uint y=0; y<4; ++y )
        {
             frustum[x][y] = 0.0f;
        }
    }
    
    visiblesList = NULL;
}


void CCCameraBase::setupViewport(float x, float y, float width, float height, const int frameBufferID)
{
    this->frameBufferID = frameBufferID;
    
    viewportX = x;
    viewportY = y;
    viewportX2 = x+width;
    viewportY2 = y+height;
    viewportWidth = width;
    viewportHeight = height;
    invViewportWidth = 1.0f / viewportWidth;
    invViewportHeight = 1.0f / viewportHeight;
    
    if( frameBufferID == -1 )
    {
        const CCTarget<float> &orientation = CCAppManager::GetOrientation();
        if( orientation.target == 270.0f )
        {
            CCSwapFloat( x, y );
            CCSwapFloat( width, height );
            x = 1.0f - width - x;
        }
        else if( orientation.target == 90.0f )
        {
            CCSwapFloat( x, y );
            CCSwapFloat( width, height );
            y = 1.0f - height - y;
        }
        else if( orientation.target == 180.0f )
        {
            x = 1.0f - width - x;
            y = 1.0f - height - y;
        }
        else
        {
        }
    }
    
    const float invY = ( 1.0f-height ) - y;
    
    const float frameBufferWidth = gEngine->renderer->frameBufferManager.getWidth( frameBufferID );
    const float frameBufferHeight = gEngine->renderer->frameBufferManager.getHeight( frameBufferID );
    const float definedWidth = width * frameBufferWidth;
    const float definedHeight = height * frameBufferHeight;
    viewport[0] = x * frameBufferWidth;
    viewport[1] = invY * frameBufferHeight;
    viewport[2] = definedWidth;
    viewport[3] = definedHeight;
    
    aspectRatio = definedWidth / definedHeight;
    
    setPerspective( perspective );
}


void CCCameraBase::recalcViewport()
{
    setupViewport( viewportX, viewportY, viewportWidth, viewportHeight, frameBufferID );
}


void CCCameraBase::setViewport()
{
    glViewport( viewport[0], viewport[1], viewport[2], viewport[3] );
    glScissor( viewport[0], viewport[1], viewport[2], viewport[3] );
}


void CCCameraBase::setNearFar(const float zNear, const float zFar)
{
    this->zNear = zNear;
    this->zFar = zFar;
    setPerspective( perspective );
}


void CCCameraBase::setPerspective(const float perspective)
{
    this->perspective = perspective;
    GluPerspective( perspective, aspectRatio );
}


bool CCCameraBase::project3D(float x, float y)
{
    if( frameBufferID == -1 )
    {
        CCAppManager::CorrectOrientation( x, y );
    }
	
	x *= viewport[2];
    x += viewport[0];
	y *= viewport[3];
    y += viewport[1];

    if( GluUnProject( x, y, 0.0f, projectionResults.vNear ) &&
        GluUnProject( x, y, 1.0f, projectionResults.vFar ) )
	{
		// Figure out our ray's direction
        projectionResults.vDirection = projectionResults.vFar;
        projectionResults.vDirection.sub( projectionResults.vNear );
        projectionResults.vDirection.unitize();
        
        projectionResults.vLookAt = projectionResults.vNear;
        CCVector3 projectionOffset = CCVector3MulResult( projectionResults.vDirection, projectionResults.vNear.z );
        projectionResults.vLookAt.add( projectionOffset );
		return true;
	}
	
	return false;
}


void CCCameraBase::project3DY(CCVector3 *result, float x, float y, float offset)
{
    if( project3D( x, y ) )
	{
		// Cast the ray from our near plane
		if( offset == -1.0f )
		{
            offset = projectionResults.vNear.y / fabsf( projectionResults.vDirection.y );
		}
	
        *result = CCVector3MulResult( projectionResults.vDirection, offset );
        result->add( projectionResults.vNear );
	}
}


void CCCameraBase::project3DZ(CCVector3 *result, float x, float y, float offset)
{
    if( project3D( x, y ) )
    {
        // Cast the ray from our near plane
        if( offset == -1.0f )
        {
            offset = projectionResults.vNear.z / fabsf( projectionResults.vDirection.z );
        }
		
        *result = CCVector3MulResult( projectionResults.vDirection, offset );
        result->add( projectionResults.vNear );
    }
}


void CCCameraBase::scanFOVBounds()
{	
    for( uint i=0; i<max_fov_scans; ++i )
    {
        if( i == scan_flat )
        {
            CCMatrixLoadIdentity( modelViewMatrix );
            CCMatrixRotate( modelViewMatrix, CCAppManager::GetOrientation().current, 0.0f, 0.0f, 1.0f );
            GluLookAt( modelViewMatrix,
                       position.x, position.y, position.z,	// Position
                       lookAt.x, lookAt.y, lookAt.z,		// Looking At
                       0.0f, 1.0f, 0.0f );					// Up
        }
        
        static CCVector3 minPlane, maxPlane;
        project3DZ( &minPlane, 0.0f, 1.0f );
        project3DZ( &maxPlane, 1.0f, 0.0f );
        
        FOVBounds &bounds = fovBounds[i];
        bounds.size.x = fabsf( maxPlane.x - minPlane.x );
        bounds.size.y = fabsf( maxPlane.y - minPlane.y );
        bounds.min.x = minPlane.x;
        bounds.min.y = minPlane.y;
        bounds.max.x = maxPlane.x;
        bounds.max.y = maxPlane.y;
    }
}


void CCCameraBase::update()
{
#if defined PROFILEON
    CCProfiler profile( "CCCameraBase::update()" );
#endif
    
    if( updatedPosition )
    {
        updatedPosition = false;
    }
    
    if( updateFOV )
    {
        updateFOV = false;
        scanFOVBounds();
    }
    
	CCMatrixLoadIdentity( modelViewMatrix );
    if( frameBufferID == -1 )
    {
        CCMatrixRotate( modelViewMatrix, CCAppManager::GetOrientation().current, 0.0f, 0.0f, 1.0f );
    }
    GluLookAt( modelViewMatrix,
			   rotatedPosition.x, rotatedPosition.y, rotatedPosition.z,	// Position
			   lookAt.x, lookAt.y, lookAt.z,                            // Looking At
			   0.0f, 1.0f, 0.0f );										// Up
	
    GLSetPushMatrix( modelViewMatrix );
	
    ExtractFrustum();
	
    updateVisibleObjects();
}


void CCCameraBase::setLookAt(const CCVector3 &newLookAt)
{
    lookAt = newLookAt;
}


void CCCameraBase::setLookAtY(const float y)
{
    setLookAt( CCVector3( lookAt.x, y, lookAt.z ) );
}


bool CCCameraBase::setLookAt(const CCVector3 &lookAtTarget, const CCVector3 &offsetTarget)
{
    if( updating )
    {
        updating = false;
        
        if( CCVector3Equals( lookAt, lookAtTarget ) == false || CCVector3Equals( offset, offsetTarget ) == false )
        {
            updatedPosition = true;
            lookAt = lookAtTarget;
            
            updateOffset( offsetTarget );
            return true;
        }
    }
    
    return false;
}


const CCVector3 CCCameraBase::getDirection()
{
    CCVector3 direction = lookAt;
    direction.sub( rotatedPosition );
    CCVector3Normalize( direction );
    return direction;
}


void CCCameraBase::updateControls()
{
    const CCScreenTouches *screenTouches = gEngine->controls->getScreenTouches();
    for( int i=0; i<CCControls::numberOfTouches; ++i )
    {
        const CCScreenTouches &screenTouch = screenTouches[i];
        CCScreenTouches &cameraTouch = cameraTouches[i];
        cameraTouch = screenTouch;
        
#ifdef DEBUGON
        if( cameraTouch.usingTouch != NULL )
        {
            cameraTouch.usingTouch = cameraTouch.usingTouch;
        }
#endif
        
        cameraTouch.startPosition.x -= viewportX;
        cameraTouch.startPosition.x *= invViewportWidth;
        cameraTouch.startPosition.y -= viewportY;
        cameraTouch.startPosition.y *= invViewportHeight;

        cameraTouch.position.x -= viewportX;
        cameraTouch.position.x *= invViewportWidth;
        cameraTouch.position.y -= viewportY;
        cameraTouch.position.y *= invViewportHeight;

        cameraTouch.delta.x *= invViewportWidth;
        cameraTouch.delta.y *= invViewportHeight;
        cameraTouch.totalDelta.x *= invViewportWidth;
        cameraTouch.totalDelta.y *= invViewportHeight;
        cameraTouch.lastTotalDelta.x *= invViewportWidth;
        cameraTouch.lastTotalDelta.y *= invViewportHeight;
    }
}


void CCCameraBase::setOffset(const CCVector3 &offsetTarget)
{
    updateOffset( offsetTarget );
}


void CCCameraBase::updateOffset(const CCVector3 &offsetTarget)
{
	offset = offsetTarget;
	
	position = lookAt;
	position.add( offset );
	rotatedPosition = position;
	
	CCRotateAboutX( rotatedPosition, rotation.x, position, lookAt ); 
	CCRotateAboutY( rotatedPosition, rotation.y, rotatedPosition, lookAt );
}


void CCCameraBase::assignVisibleObjects(CCList<CCObjectCollideable> *inVisiblesList)
{
    visiblesList = inVisiblesList;
}


void CCCameraBase::updateVisibleObjects()
{	
#if defined PROFILEON
    CCProfiler profile( "CCCameraBase::updateVisibleObjects()" );
#endif
    
    // Update visible objects
    if( visiblesList )
    {
        CCScanVisibleCollideables( frustum, *visiblesList, visibleCollideables );
    }
    else
    {
        CCOctreeScanVisibleCollideables( frustum, visibleCollideables );
    }
    
    for( int i=0; i<visibleCollideables.length; ++i )
    {
        sortedVisibleCollideables[i] = i;
    }
}


void CCCameraBase::setRotationX(const float inRotation)
{
    rotation.x = inRotation;
    updating = true;
}


void CCCameraBase::setRotationY(const float inRotation)
{
    rotation.y = inRotation;
    updating = true;
}


void CCCameraBase::setRotationZ(const float inRotation)
{
    rotation.z = inRotation;
    updating = true;
}


void CCCameraBase::incrementRotationX(const float increment)
{
    rotation.x += increment;
    setRotationX( rotation.x );
}


void CCCameraBase::incrementRotationY(const float increment)
{
    rotation.y += increment;
    setRotationY( rotation.y );
}


void CCCameraBase::incrementRotationZ(const float increment)
{
    rotation.z += increment;
    setRotationZ( rotation.z );
}


// glu based camera functionality
void CCCameraBase::GluPerspective(float fovy, float aspect)
{	
    frustumMax.y = zNear * tanf( fovy * CC_PI / 360.0f );
	frustumMin.y = -frustumMax.y;
	frustumMin.x = frustumMin.y * aspect;
	frustumMax.x = frustumMax.y * aspect;
	
	const float zNearScale = 1.0f / zNear;
	frustumSize.width = ( frustumMax.x + -frustumMin.x ) * zNearScale;
	frustumSize.height = ( frustumMax.y + -frustumMin.y ) * zNearScale;
	
    if( gEngine->renderer->openGL2() )
    {
        CCMatrixLoadIdentity( projectionMatrix );
        CCMatrixFrustum( projectionMatrix, frustumMin.x, frustumMax.x, frustumMin.y, frustumMax.y, zNear, zFar );
    }
    else
    {
#ifdef IOS
        
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glFrustumf( frustumMin.x, frustumMax.x, frustumMin.y, frustumMax.y, zNear, zFar );
        glGetFloatv( GL_PROJECTION_MATRIX, projectionMatrix.data() );
        
#endif
    }
}


void CCCameraBase::GluLookAt(CCMatrix &modelViewMatrix, 
                             float eyex, float eyey, float eyez,			   
                             float centerx, float centery, float centerz,
                             float upx, float upy, float upz)
{
	float x[3], y[3], z[3];
	float mag;
	
	/* Make rotation matrix */
	
	/* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrtf( z[0] * z[0] + z[1] * z[1] + z[2] * z[2] );
	if( mag )
	{	/* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}
	
	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;
	
	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];
	
	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];
	
	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */
	
	mag = sqrtf( x[0] * x[0] + x[1] * x[1] + x[2] * x[2] );
	if( mag )
	{
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}
	
	mag = sqrtf( y[0] * y[0] + y[1] * y[1] + y[2] * y[2] );
	if( mag )
	{
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}
	
	CCMatrix m;
    // TODO: Remove define and feed directly into matrix.m
#define M(row, col) m.data()[col*4+row]
	M(0,0) = x[0];
	M(0,1) = x[1];
	M(0,2) = x[2];
	M(0,3) = 0.0f;
	M(1,0) = y[0];
	M(1,1) = y[1];
	M(1,2) = y[2];
	M(1,3) = 0.0f;
	M(2,0) = z[0];
	M(2,1) = z[1];
	M(2,2) = z[2];
	M(2,3) = 0.0f;
	M(3,0) = 0.0f;
	M(3,1) = 0.0f;
	M(3,2) = 0.0f;
	M(3,3) = 1.0f;
#undef M
	CCMatrixMultiply( modelViewMatrix, m, modelViewMatrix );
	
	/* Translate Eye to Origin */
	CCMatrixTranslate( modelViewMatrix, -eyex, -eyey, -eyez );
}


static bool gluUnProject(float winX, float winY, float winZ,
                               const float modelMatrix[16], 
                               const float projMatrix[16],
                               const int viewport[4],
                               float *objX, float *objY, float *objZ)
{
	float finalMatrix[16];
	float in[4];
	float out[4];
	
	CCMatrixMulMat( modelMatrix, projMatrix, finalMatrix );
	if( CCMatrixInvert( finalMatrix, finalMatrix ) == false ) 
	{
		return false;
	}
	
	in[0] = winX;
	in[1] = winY;
	in[2] = winZ;
	in[3] = 1.0f;
	
	/* Map x and y from window coordinates */
	in[0] = ( in[0] - viewport[0] ) / viewport[2];
	in[1] = ( in[1] - viewport[1] ) / viewport[3];
	
	/* Map to range -1 to 1 */
	in[0] = in[0] * 2 - 1;
	in[1] = in[1] * 2 - 1;
	in[2] = in[2] * 2 - 1;
	
	CCMatrixMulVec( finalMatrix, in, out );
	if( out[3] == 0.0 )
	{
		return false;
	}
	
	out[0] /= out[3];
	out[1] /= out[3];
	out[2] /= out[3];
	*objX = out[0];
	*objY = out[1];
	*objZ = out[2];
	
	return true;
}


bool CCCameraBase::GluUnProject(const float x, const float y, const float z, CCVector3 &result)
{
    return gluUnProject( x, y, z, getModelViewMatrix().data(), projectionMatrix.data(), viewport, &result.x, &result.y, &result.z );
}


static bool gluProject(float objX, float objY, float objZ, 
                       const float modelMatrix[16], 
                       const float projMatrix[16],
                       const int viewport[4],
                       float *winX, float *winY, float *winZ,
                       const bool correctOrientation=true)
{
    float in[4];
    float out[4];
	
    in[0] = objX;
    in[1] = objY;
    in[2] = objZ;
    in[3] = 1.0f;
    
	CCMatrixMulVec( modelMatrix, in, out );
    CCMatrixMulVec( projMatrix, out, in );
    
	if( in[3] == 0.0)
	{
		return false;
	}
	
    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];
    /* Map x, y and z to range 0-1 */
    in[0] = in[0] * 0.5f + 0.5f;
    in[1] = in[1] * 0.5f + 0.5f;
    in[2] = in[2] * 0.5f + 0.5f;
	
    /* Map x,y to viewport */
    //in[0] = in[0] * viewport[2] + viewport[0];
    //in[1] = in[1] * viewport[3] + viewport[1];
	
	in[1] = 1.0f - in[1];
	
	if( in[3] < 0.0 )
	{
		in[0] *= -1.0f;
		in[1] *= -1.0f;
	}
    
    if( correctOrientation )
    {
        CCAppManager::CorrectOrientation( in[0], in[1] );
    }
	
    *winX = in[0];
    *winY = in[1];
    *winZ = in[2];
	
    return true;
}


bool CCCameraBase::GluProject(CCRenderable *object, CCVector3 &result)
{
	return gluProject( object->positionPtr->x, object->positionPtr->y, object->positionPtr->z,
					   getModelViewMatrix().data(),
					   projectionMatrix.data(),
					   viewport,
					   &result.x, &result.y, &result.z,
                       frameBufferID==-1 );
}


void CCCameraBase::ExtractFrustum()
{
	float   *proj = projectionMatrix.data();
	float   *modv = getModelViewMatrix().data();
	float   clip[16];
	float   t;
	
	/* Get the current MODELVIEW matrix from OpenGL */
	//glGetFloatv( GL_MODELVIEW_MATRIX, modv );
	
	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modv[ 0] * proj[ 0] + modv[ 1] * proj[ 4] + modv[ 2] * proj[ 8] + modv[ 3] * proj[12];
	clip[ 1] = modv[ 0] * proj[ 1] + modv[ 1] * proj[ 5] + modv[ 2] * proj[ 9] + modv[ 3] * proj[13];
	clip[ 2] = modv[ 0] * proj[ 2] + modv[ 1] * proj[ 6] + modv[ 2] * proj[10] + modv[ 3] * proj[14];
	clip[ 3] = modv[ 0] * proj[ 3] + modv[ 1] * proj[ 7] + modv[ 2] * proj[11] + modv[ 3] * proj[15];
	
	clip[ 4] = modv[ 4] * proj[ 0] + modv[ 5] * proj[ 4] + modv[ 6] * proj[ 8] + modv[ 7] * proj[12];
	clip[ 5] = modv[ 4] * proj[ 1] + modv[ 5] * proj[ 5] + modv[ 6] * proj[ 9] + modv[ 7] * proj[13];
	clip[ 6] = modv[ 4] * proj[ 2] + modv[ 5] * proj[ 6] + modv[ 6] * proj[10] + modv[ 7] * proj[14];
	clip[ 7] = modv[ 4] * proj[ 3] + modv[ 5] * proj[ 7] + modv[ 6] * proj[11] + modv[ 7] * proj[15];
	
	clip[ 8] = modv[ 8] * proj[ 0] + modv[ 9] * proj[ 4] + modv[10] * proj[ 8] + modv[11] * proj[12];
	clip[ 9] = modv[ 8] * proj[ 1] + modv[ 9] * proj[ 5] + modv[10] * proj[ 9] + modv[11] * proj[13];
	clip[10] = modv[ 8] * proj[ 2] + modv[ 9] * proj[ 6] + modv[10] * proj[10] + modv[11] * proj[14];
	clip[11] = modv[ 8] * proj[ 3] + modv[ 9] * proj[ 7] + modv[10] * proj[11] + modv[11] * proj[15];
	
	clip[12] = modv[12] * proj[ 0] + modv[13] * proj[ 4] + modv[14] * proj[ 8] + modv[15] * proj[12];
	clip[13] = modv[12] * proj[ 1] + modv[13] * proj[ 5] + modv[14] * proj[ 9] + modv[15] * proj[13];
	clip[14] = modv[12] * proj[ 2] + modv[13] * proj[ 6] + modv[14] * proj[10] + modv[15] * proj[14];
	clip[15] = modv[12] * proj[ 3] + modv[13] * proj[ 7] + modv[14] * proj[11] + modv[15] * proj[15];
	
	/* Extract the numbers for the RIGHT plane */
	frustum[frustum_right][0] = clip[ 3] - clip[ 0];
	frustum[frustum_right][1] = clip[ 7] - clip[ 4];
	frustum[frustum_right][2] = clip[11] - clip[ 8];
	frustum[frustum_right][3] = clip[15] - clip[12];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_right][0] * frustum[frustum_right][0] + frustum[frustum_right][1] * frustum[frustum_right][1] + frustum[frustum_right][2] * frustum[frustum_right][2] );
	frustum[frustum_right][0] /= t;
	frustum[frustum_right][1] /= t;
	frustum[frustum_right][2] /= t;
	frustum[frustum_right][3] /= t;
	
	/* Extract the numbers for the LEFT plane */
	frustum[frustum_left][0] = clip[ 3] + clip[ 0];
	frustum[frustum_left][1] = clip[ 7] + clip[ 4];
	frustum[frustum_left][2] = clip[11] + clip[ 8];
	frustum[frustum_left][3] = clip[15] + clip[12];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_left][0] * frustum[frustum_left][0] + frustum[frustum_left][1] * frustum[frustum_left][1] + frustum[frustum_left][2] * frustum[frustum_left][2] );
	frustum[frustum_left][0] /= t;
	frustum[frustum_left][1] /= t;
	frustum[frustum_left][2] /= t;
	frustum[frustum_left][3] /= t;
	
	/* Extract the BOTTOM plane */
	frustum[frustum_bottom][0] = clip[ 3] + clip[ 1];
	frustum[frustum_bottom][1] = clip[ 7] + clip[ 5];
	frustum[frustum_bottom][2] = clip[11] + clip[ 9];
	frustum[frustum_bottom][3] = clip[15] + clip[13];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_bottom][0] * frustum[frustum_bottom][0] + frustum[frustum_bottom][1] * frustum[frustum_bottom][1] + frustum[frustum_bottom][2] * frustum[frustum_bottom][2] );
	frustum[frustum_bottom][0] /= t;
	frustum[frustum_bottom][1] /= t;
	frustum[frustum_bottom][2] /= t;
	frustum[frustum_bottom][3] /= t;
	
	/* Extract the TOP plane */
	frustum[frustum_top][0] = clip[ 3] - clip[ 1];
	frustum[frustum_top][1] = clip[ 7] - clip[ 5];
	frustum[frustum_top][2] = clip[11] - clip[ 9];
	frustum[frustum_top][3] = clip[15] - clip[13];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_top][0] * frustum[frustum_top][0] + frustum[frustum_top][1] * frustum[frustum_top][1] + frustum[frustum_top][2] * frustum[frustum_top][2] );
	frustum[frustum_top][0] /= t;
	frustum[frustum_top][1] /= t;
	frustum[frustum_top][2] /= t;
	frustum[frustum_top][3] /= t;
	
	/* Extract the FAR plane */
	frustum[frustum_far][0] = clip[ 3] - clip[ 2];
	frustum[frustum_far][1] = clip[ 7] - clip[ 6];
	frustum[frustum_far][2] = clip[11] - clip[10];
	frustum[frustum_far][3] = clip[15] - clip[14];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_far][0] * frustum[frustum_far][0] + frustum[frustum_far][1] * frustum[frustum_far][1] + frustum[frustum_far][2] * frustum[frustum_far][2] );
	frustum[frustum_far][0] /= t;
	frustum[frustum_far][1] /= t;
	frustum[frustum_far][2] /= t;
	frustum[frustum_far][3] /= t;
	
	/* Extract the NEAR plane */
	frustum[frustum_near][0] = clip[ 3] + clip[ 2];
	frustum[frustum_near][1] = clip[ 7] + clip[ 6];
	frustum[frustum_near][2] = clip[11] + clip[10];
	frustum[frustum_near][3] = clip[15] + clip[14];
	
	/* Normalize the result */
	t = sqrtf( frustum[frustum_near][0] * frustum[frustum_near][0] + frustum[frustum_near][1] * frustum[frustum_near][1] + frustum[frustum_near][2] * frustum[frustum_near][2] );
	frustum[frustum_near][0] /= t;
	frustum[frustum_near][1] /= t;
	frustum[frustum_near][2] /= t;
	frustum[frustum_near][3] /= t;
}


void GLSetPushMatrix(CCMatrix &matrix)
{
    CCCameraBase::currentCamera->currentPush = 0;
    CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush] = matrix;
}


void GLPushMatrix()
{
    CCCameraBase::currentCamera->currentPush++;
    CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush] = CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush-1];
    ASSERT( CCCameraBase::currentCamera->currentPush < MAX_PUSHES );
}

void GLPopMatrix()
{
    CCCameraBase::currentCamera->currentPush--;
}


void GLLoadIdentity()
{
    CCMatrix identityMatrix;
    CCMatrixLoadIdentity( identityMatrix );
    CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush] = identityMatrix;
}


void GLMultMatrixf(CCMatrix &matrix)
{
    CCMatrixMultiply( CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush], matrix, CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush] );
}


void GLTranslatef(GLfloat tx, GLfloat ty, GLfloat tz)
{
    CCMatrixTranslate( CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush], tx, ty, tz );
}


void GLScalef(GLfloat sx, GLfloat sy, GLfloat sz)
{
    CCMatrixScale( CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush], sx, sy, sz );
}


void GLRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    CCMatrixRotate( CCCameraBase::currentCamera->pushedMatrix[CCCameraBase::currentCamera->currentPush], angle, x, y, z );
}