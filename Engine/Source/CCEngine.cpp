/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCEngine.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCTextureFontPage.h"
#include "CCObjects.h"
#include "CCSceneBase.h"
#include "CCFileManager.h"

#include "CCDeviceControls.h"
#include "CCDeviceRenderer.h"
#include "CCAppManager.h"

#ifdef IOS
#include <OpenGLES/ES1/gl.h>
#endif


CCEngine::CCEngine() :
    collisionManager( 250000.0f )
{
    running = true;
    engineThreadRunning = false;
    paused = false;
    pauseRendering = false;
    
    renderer = NULL;
    textureManager = NULL;
    
	renderFlags = render_all;
	fpsLimit = 1/50.0f;
    
    // Initialise our time lastUpdate value;
    updateTime();
}


CCEngine::~CCEngine()
{		
	scenes.deleteObjectsAndList( true );
	cameras.deleteObjectsAndList();
	
	delete textureManager;
	delete controls;
	delete renderer;

	gEngine = NULL;
	
    CCNativeThreadUnlock();
}


void CCEngine::setupNativeThread()
{
}


static int zCompare(const void *a, const void *b)
{
    const CCObjectCollideable *objectA = CCOctreeGetVisibleCollideables( *(int*)a );
    const CCObjectCollideable *objectB = CCOctreeGetVisibleCollideables( *(int*)b );
    
    if( objectA->drawOrder == 200 || objectB->drawOrder == 200 )
    {
        if( objectA->drawOrder == 200 && objectB->drawOrder == 200 )
        {
            const CCVector3 &cameraPosition = CCCameraBase::currentCamera->getRotatedPosition();
            const CCVector3 *positionA = objectA->positionPtr;
            const CCVector3 *positionB = objectB->positionPtr;
            const float distanceA = CCVector3Distance( *positionA, cameraPosition, true );
            const float distanceB = CCVector3Distance( *positionB, cameraPosition, true );
            
            // If A is smaller than B, swap
            return distanceB - distanceA;
        }
        
        // Largest to the back to be drawn last
        return objectA->drawOrder - objectB->drawOrder;
    }
    
    // Largest to the back to be drawn last
    return objectA->drawOrder - objectB->drawOrder;
}


bool CCEngine::setupEngineThread()
{
	CCOctreeSetRenderSortCallback( &zCompare );
    
    const bool rendererSetup = setupRenderer();
    
    controls = new CCDeviceControls();
    
    if( rendererSetup )
    {
        start();
    }
    
    engineThreadRunning = true;
    return rendererSetup;
}


bool CCEngine::setupRenderer()
{   
    if( textureManager != NULL )
    {
        textureManager->unloadAllTextures();
    }
    
    if( renderer != NULL )
    {
        delete renderer;
    }
    
    renderer = new CCDeviceRenderer();
    const bool rendererSetup = renderer->setup( true );
    DEBUG_OPENGL();
    
    if( textureManager == NULL )
    {
        textureManager = new CCTextureManager();
    }
    DEBUG_OPENGL();
    
    if( rendererSetup )
    {
        textureManager->load();
        DEBUG_OPENGL();
    
        renderer->setupOpenGL();
        DEBUG_OPENGL();
    }
    
    return rendererSetup;
}


void CCEngine::addCamera(CCCameraBase *camera, const int index)
{
    camera->setIndex( cameras.length );
    cameras.add( camera );

    if( index != -1 )
    {
        cameras.reinsert( camera, index );
    }
}


bool CCEngine::removeCamera(CCCameraBase *camera)
{
	if( cameras.remove( camera ) )
	{
		return true;
	}
	return false;
}


void CCEngine::refreshCameras()
{
    for( int i=0; i<cameras.length; ++i )
    {
        CCCameraBase *camera = cameras.list[i];
        camera->recalcViewport();
    }
}


void CCEngine::addScene(CCSceneBase *scene, const int index)
{
	scenes.add( scene );
	scene->setup();
    
    if( index != -1 )
    {
        scenes.reinsert( scene, index );
    }
}


void CCEngine::removeScene(CCSceneBase* scene)
{
	if( scenes.remove( scene ) )
	{
		DELETE_OBJECT( scene );
	}
    else
    {
        ASSERT( false );
    }
}


const double getSystemTime()
{
#ifdef IOS
    
    const NSTimeInterval currentTime = [NSDate timeIntervalSinceReferenceDate];
    
#elif defined( QT )
    
    const double currentTime = gView->timer.elapsed() * 0.001;
    
#elif defined( ANDROID )
    
    struct timespec res;
	clock_gettime( CLOCK_REALTIME, &res );
	const double currentTime = res.tv_sec + (double)( ( res.tv_nsec / 1e6 ) * 0.001 );

#endif

    return currentTime;
}


bool CCEngine::updateTime()
{
    double currentTime = getSystemTime();
    time.real = (float)( currentTime - time.lastUpdate );
	
    // If we're too fast, sleep
	while( time.real < fpsLimit )
	{
        const uint difference = roundf( ( fpsLimit - time.real ) * 1000.0f ) + 1;
		usleep( difference );

		currentTime = getSystemTime();
		time.real = (float)( currentTime - time.lastUpdate );
	}
	
	// Fake 25 fps
    static const float minFPS = 1/15.0f;
	time.delta = MIN( time.real, minFPS );

    time.lastUpdate = currentTime;
    
    return true;
}


bool CCEngine::updateNativeThread()
{	
    // Run callbacks
	if( nativeThreadCallbacks.length > 0 )
    {
        CCNativeThreadLock();
        for( int i=0; i<nativeThreadCallbacks.length; ++i )
        {
            nativeThreadCallbacks.list[i]->run();
            delete nativeThreadCallbacks.list[i];
        }
        nativeThreadCallbacks.length = 0;
        CCNativeThreadUnlock();
    }

	return false;
}


void CCEngine::updateEngineThread()
{	
    // Update our system time
    if( updateTime() == false )
    {
#ifdef ANDROID
    	// FIXME: Android needs to always redraw the scene
    	// We currently never return false, so perhaps remove this..
    	renderer->clear();
        renderLoop();
#endif
        return;
    }

	time.lifetime += time.real;

#if LOG_FPS
    static uint loggedUpdates = 0;
    static float loggedDelta = 0.0f;
    loggedUpdates++;
    loggedDelta += time.real;
    if( loggedDelta > 1.0f )
    {
        const float averageFPS = 1.0f / ( loggedDelta / loggedUpdates );
        DEBUGLOG( "Average FPS: %f \n", averageFPS );
        loggedUpdates = 0;
        loggedDelta = 0.0f;
    }
#endif
    
    // Run callbacks
    if( engineThreadCallbacks.length > 0 )
    {
        CCNativeThreadLock();
        CCJobsThreadLock();
        
        while( engineThreadCallbacks.length > 0 )
        {
            CCLambdaCallback *callback = engineThreadCallbacks.pop();
            callback->run();
            delete callback;
        }
        
        CCNativeThreadUnlock();
        CCJobsThreadUnlock();
    }
	
    finishJobs();
	updateLoop();
    
    if( pauseRendering == false )
    {
        CCAppManager::UpdateOrientation( time.delta );
        
        renderer->clear();
        renderLoop();
        renderer->resolve();
    }
	
#if defined DEBUGON && TARGET_IPHONE_SIMULATOR
	// 66 frames a second in debug
	//usleep( 15000 );
	usleep( 0 );
#endif
}


bool CCEngine::updateJobsThread()
{	
    // Run callbacks
	if( jobsThreadCallbacks.length > 0 )
    {
        CCJobsThreadLock();
        CCLambdaCallback *callback = jobsThreadCallbacks.pop();
        CCJobsThreadUnlock();
        callback->run();
        delete callback;
        return true;
    }
    return false;
}


void CCEngine::updateLoop()
{	
#if defined PROFILEON
    CCProfiler profile( "CCEngine::updateGame()" );
#endif
    
    // Remove any redundant scenes
    for( int i=0; i<scenes.length; ++i )
	{
		CCSceneBase *scene = scenes.list[i];
        if( scene->shouldDelete() )
		{
            removeScene( scene );
			i--;
		}
	}
    
    CCNativeThreadLock();
    for( int i=0; i<cameras.length; ++i )
    {
        CCCameraBase *camera = cameras.list[i];
        camera->updateControls();
    }
	controls->update( time );
    CCNativeThreadUnlock();
    
    // Allow scene to handle the controls first
	for( int i=scenes.length-1; i>=0; --i )
    {
        if( scenes.list[i]->updateControls( time ) )
        {
            break;
        }
	}

    // Run through our controllers first
    for( int i=0; i<updaters.length; ++i )
    {
        // Delete on finish update
        CCUpdater *updater = updaters.list[i];
        if( updater->update( time.delta ) == false )
        {
            updaters.remove( updater );
            updater->finish();
            DELETE_OBJECT( updater );
            --i;
        }
    }
	
    // Update scene logic
	for( int i=0; i<scenes.length; ++i )
    {
        CCSceneBase *scene = scenes.list[i];
        scene->update( time );
	}
}


void CCEngine::renderLoop()
{    
#if defined PROFILEON
    CCProfiler profile( "CCEngine::render()" );
#endif
    
    // Tell the texture manager we're rendering a new frame
    gEngine->textureManager->prepareRender();
    {
        CCList<CCObjectCollideable> &collideables = collisionManager.collideables;
        for( int i=0; i<collideables.length; ++i )
        {
            collideables.list[i]->visible = false;
        }
    }
    
	for( int cameraIndex=0; cameraIndex<cameras.length; ++cameraIndex )
    {
		CCCameraBase *currentCamera = CCCameraBase::currentCamera = cameras.list[cameraIndex];
		if( currentCamera->isEnabled() == false )
		{
			continue;
		}
        
        if( currentCamera->getFrameBufferID() != -1 )
        {
            continue;
        }


#if defined PROFILEON
		CCProfiler profile( "CCEngine::render()::camera" );
#endif

        currentCamera->setViewport();
		
		// 2D Background
        if( false )
		{	
			CCMatrix matrix;
			CCMatrixLoadIdentity( matrix );
			CCMatrixRotate( matrix, CCAppManager::GetOrientation().current, 0.0f, 0.0f, 1.0f );
			CCMatrixOrtho( matrix, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f );
			GLSetPushMatrix( matrix );
			
			for( int i=0; i<scenes.length; ++i )
			{
				scenes.list[i]->render2DBackground( cameraIndex );
			}
		}
		
		// 3D Rendering
		GLEnableDepth();
        GLUseProjectionMatrix( true );
		{	
            currentCamera->update();
            if( renderer->openGL2() == false )
            {
#ifdef IOS
                
                glMatrixMode( GL_PROJECTION );
                currentCamera->setPerspective( 60.0f );
                glMatrixMode( GL_MODELVIEW );
                
#endif 
            }
			
			for( uint pass=render_background; pass<render_finished; ++pass )
			{
#if defined PROFILEON
				CCProfiler profile( "CCEngine::render()::pass" );
#endif

				// Render all the non-collideables
				GLDisableBlend();
				for( int i=0; i<scenes.length; ++i )
				{
					scenes.list[i]->render( currentCamera, pass, false );
				}
				
				// Render all the visible collideables
				{
#if defined PROFILEON
					CCProfiler profile( "CCOctreeRenderVisibleObjects( false )" );
#endif
					CCOctreeRenderVisibleObjects( currentCamera, pass, false );
				}
				
				GLEnableBlend();
				
				if( pass == render_main )
				{	
					if( CCHasFlag( gEngine->renderFlags, render_collisionTrees ) )
					{
						CCOctreeRender( collisionManager.tree );
					}
                    
                    if( CCHasFlag( gEngine->renderFlags, render_pathFinder ) )
					{
						collisionManager.pathFinderNetwork.view();
					}

					// Keep it disabled for the rest of the renders
					GLDisableDepth();
				}
				
				for( int i=0; i<scenes.length; ++i )
				{
					scenes.list[i]->render( currentCamera, pass, true );
				}
				
				// Render all the visible collideables
				{

#if defined PROFILEON
					CCProfiler profile( "CCOctreeRenderVisibleObjects( true )" );
#endif
					CCOctreeRenderVisibleObjects( currentCamera, pass, true );
				}
			}
		}
        GLUseProjectionMatrix( false );
		
		// 2D Rendering
        if( false )
		{
			CCMatrix matrix;
			CCMatrixLoadIdentity( matrix );
			CCMatrixRotate( matrix, CCAppManager::GetOrientation().current, 0.0f, 0.0f, 1.0f );
			CCMatrixOrtho( matrix, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f );
			GLSetPushMatrix( matrix );
			
            for( int i=0; i<scenes.length; ++i )
            {
                scenes.list[i]->render2DForeground( cameraIndex );
            }
			
#ifdef DEBUGON
			if( CCHasFlag( gEngine->renderFlags, render_fontPage ) )
			{
				CCSetColour( CCColour() );
				textureManager->fontPages.list[0]->view();
			}
#endif
		}
		
		currentCamera = NULL;
	}
}


void CCEngine::renderFrameBuffer(const int frameBufferID)
{  
    if( renderer->openGL2() == false )
    {
        return;
    }
    
    if( frameBufferID == -1 )
    {
        return;
    }
    
    CCCameraBase *defaultFrameBufferCamera = CCCameraBase::currentCamera;
    const bool defaultBlendState = GLBlendState();
    const bool defaultDepthState = GLDepthState();
    
    bool bindedFrameBuffer = false;
    for( int cameraIndex=0; cameraIndex<cameras.length; ++cameraIndex )
    {
        CCCameraBase *currentCamera = CCCameraBase::currentCamera = cameras.list[cameraIndex];
        if( currentCamera->isEnabled() == false )
        {
            continue;
        }
        
        if( currentCamera->getFrameBufferID() != frameBufferID )
        {
            continue;
        }
        
        if( bindedFrameBuffer == false )
        {
            bindedFrameBuffer = true;
            renderer->frameBufferManager.bindFrameBuffer( frameBufferID );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        }
        currentCamera->setViewport();
        
        // 3D Rendering
		GLEnableDepth();
        {	
            currentCamera->update();
            
            for( uint pass=render_background; pass<render_finished; ++pass )
            {   
                // Render all the non-collideables
                GLDisableBlend();
                for( int i=0; i<scenes.length; ++i )
                {
                    scenes.list[i]->render( currentCamera, pass, false );
                }
                
                // Render all the visible collideables
                {
                    CCOctreeRenderVisibleObjects( currentCamera, pass, false );
                }
                
                GLEnableBlend();
                if( pass == render_main )
                {
                    // Keep it disabled for the rest of the renders
                    GLDisableDepth();
                }
                
                for( int i=0; i<scenes.length; ++i )
                {
                    scenes.list[i]->render( currentCamera, pass, true );
                }
                
                // Render all the visible collideables
                {
                    CCOctreeRenderVisibleObjects( currentCamera, pass, true );
                }
            }
        }
    }
    
    // Generate a mip map - too slow for use?
    if( bindedFrameBuffer )
    {
        // TODO: Should be using glew to load func pointer, but mipmap isn't too important while Windows is not a target platform.
#if !defined( ANDROID ) && !defined( Q_OS_WIN ) && !defined( Q_OS_LINUX )
        glBindTexture( GL_TEXTURE_2D, renderer->frameBufferManager.getFrameBufferTexture( frameBufferID ) );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glGenerateMipmap( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, textureManager->getCurrentGLTexture() );
#endif
    }
    
    if( bindedFrameBuffer )
    {
        renderer->frameBufferManager.bindDefaultFrameBuffer();
    }
    CCCameraBase::currentCamera = defaultFrameBufferCamera;
    CCCameraBase::currentCamera->setViewport();
    
    if( defaultBlendState )
    {
        GLEnableBlend();
    }
    else
    {
        GLDisableBlend();
    }
    if( defaultDepthState )
    {
        GLEnableDepth();
    }
    else
    {
        GLDisableDepth();
    }
}


void CCEngine::finishJobs()
{	
#if defined PROFILEON
    CCProfiler profile( "CCEngine::finishJobs()" );
#endif
    
    CCFileManager::ReadyIO();
    
	// Prune the octree
	if( collisionManager.pruneTreesTimer > 0.0f )
	{
		collisionManager.pruneTreesTimer -= time.real;
		if( collisionManager.pruneTreesTimer <= 0.0f )
		{
            //DEBUGLOG( "Octree - prune" );
			CCOctreePruneTree( collisionManager.tree );
		}
	}
}


void CCEngine::restart()
{
    for( int i=scenes.length-1; i>=0; --i )
    {
        removeScene( scenes.list[i] );
    }
    
    start();
}


void CCEngine::addCollideable(CCObjectCollideable* collideable)
{
	collisionManager.collideables.add( collideable );
	CCOctreeAddObject( collisionManager.tree, collideable );
}


void CCEngine::removeCollideable(CCObjectCollideable* collideable)
{	
    collisionManager.collideables.remove( collideable );
	CCOctreeRemoveObject( collideable );
}


bool CCEngine::serialize(const bool saving)
{
	return false;
}


bool CCEngine::isOrientationSupported(const float angle)
{
    return true;
}


void CCEngine::nextEngineUpdate(CCLambdaCallback *lambdaCallback, const int index)
{
    engineThreadCallbacks.add( lambdaCallback );
    
    if( index >= 0 )
    {
        engineThreadCallbacks.reinsert( lambdaCallback, index );
    }
}


void CCEngine::engineToNativeThread(CCLambdaCallback *lambdaCallback)
{
    CCNativeThreadLock();
    nativeThreadCallbacks.add( lambdaCallback );
    CCNativeThreadUnlock();
}


void CCEngine::nativeToEngineThread(CCLambdaCallback *lambdaCallback)
{
    CCNativeThreadLock();
    nextEngineUpdate( lambdaCallback, 0 );
    CCNativeThreadUnlock();
}


void CCEngine::engineToJobsThread(CCLambdaCallback *lambdaCallback, const bool pushToFront)
{
    CCJobsThreadLock();
    jobsThreadCallbacks.add( lambdaCallback );
    if( pushToFront )
    {
        jobsThreadCallbacks.reinsert( lambdaCallback, 0 );
    }
    CCJobsThreadUnlock();
}


void CCEngine::jobsToEngineThread(CCLambdaCallback *lambdaCallback)
{
    CCJobsThreadLock();
    engineThreadCallbacks.add( lambdaCallback );
    CCJobsThreadUnlock();
}


void CCEngine::pause()
{
    paused = true;
    
    for( int i=0; i<scenes.length; ++i )
    {
        scenes.list[i]->appPaused();
    }
}


void CCEngine::resume()
{
    paused = false;
    
    for( int i=0; i<scenes.length; ++i )
    {
        scenes.list[i]->appResumed();
    }
}
