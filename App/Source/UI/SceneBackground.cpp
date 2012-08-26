/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneBackground.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"

#include "CCAppManager.h"
#include "CCTextureBase.h"


SceneBackground::SceneBackground()
{   
    cameraCentered = true;

    camera = new CCCameraAppUI();
    gEngine->addCamera( camera );
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
}


void SceneBackground::destruct()
{
    super::destruct();
}


void SceneBackground::setup()
{
    CCText texPath = "Resources/";
    texPath += CLIENT_NAME;
    texPath += "/uimenu/ui_background.png";
    
    const float backgroundWidth = gEngine->textureManager->getTexture( texPath.buffer )->getRawWidth();
    camera->setCameraWidth( backgroundWidth, false );
    
    // Create background
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupTextured( texPath.buffer, Resource_Packaged, true );
        tile->setTileScale( 1.0f );
        tile->setTouchDepressDepth( 0.0f );
        tileBackground = tile;
        
        addTile( tile );
        
        LAMBDA_CONNECT_CLASS( tile->onRelease, ScenePlayManager, ScenePlayManager::scene, start(); );
    }    
    show();

    refreshCameraView();
    lockCameraView();
}


// CCSceneBase
bool SceneBackground::updateScene(const CCTime &time)
{
    bool updated = super::updateScene( time );
    
    return updated;
}


bool SceneBackground::updateCamera(const CCTime &time)
{
    return super::updateCamera( time );
}


bool SceneBackground::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // All objects go through the Octree render pipe here
    return false;
}


void SceneBackground::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->renderObject( inCamera, alpha );
	}
}


void SceneBackground::beginOrientationUpdate()
{
    super::beginOrientationUpdate();
}


bool SceneBackground::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool SceneBackground::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    return false;
}


bool SceneBackground::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    return super::touchReleased( touch, touchAction );
}


bool SceneBackground::touchCameraRotating(const float x, const float y)
{
    return false;
}


bool SceneBackground::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void SceneBackground::show()
{   
    tileBackground->setTileColourAlpha( 1.0f, true );
    tileBackground->enabled = true;
}


void SceneBackground::hide()
{
    tileBackground->setTileColourAlpha( 0.0f, true );
    tileBackground->enabled = false;
}