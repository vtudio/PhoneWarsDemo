/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameUI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"

#include "CCAppManager.h"


SceneGameUI::SceneGameUI(SceneGameSyndicate *sceneGame)
{
    cameraCentered = true;
    
    this->sceneGame = sceneGame;
    linkScene( sceneGame );
    sceneGame->linkScene( this );
    
    camera = new CCCameraAppUI();
    gEngine->addCamera( camera );
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
}


void SceneGameUI::destruct()
{
    super::destruct();
}


void SceneGameUI::setup()
{
    camera->setCameraWidth( 100.0f, false );
    
    for( int i=0; i<2; ++i )
    {
        // Lives
        {
            CCTile3DButton *tile = new CCTile3DButton( this );
            tile->setupTexturedWidth( camera->cameraWidth * 0.1f, "Resources/common/uigame/ui_icon_life.png", Resource_Packaged, true );
            tile->getTileScaleInterpolator().setDuration( 0.25f );
            tileHealth[i] = tile;
        }
    }

    // refresh the scene tile positioning and range
    beginOrientationUpdate();
}


// CCSceneBase
bool SceneGameUI::updateScene(const CCTime &time)
{
    bool updated = super::updateScene( time );
    return updated;
}


bool SceneGameUI::updateCamera(const CCTime &time)
{
    return super::updateCamera( time );
}


bool SceneGameUI::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // All objects go through the Octree render pipe here
    return false;
}


void SceneGameUI::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->renderObject( inCamera, alpha );
	}
}


void SceneGameUI::beginOrientationUpdate()
{
    super::beginOrientationUpdate();
    
    {
        CCTile3DButton *tile = tileHealth[0];
        tile->setPositionXYZ( -camera->cameraHWidth + tile->collisionBounds.x, 
                              -camera->cameraHHeight + tile->collisionBounds.y, 
                              0.0f );
    }
    {
        CCTile3DButton *tile = tileHealth[1];
        tile->setPositionXYZ( camera->cameraHWidth - tile->collisionBounds.x, 
                             -camera->cameraHHeight + tile->collisionBounds.y, 
                             0.0f );
    }
}


bool SceneGameUI::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool SceneGameUI::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    return false;
}


bool SceneGameUI::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    return super::touchReleased( touch, touchAction );
}


bool SceneGameUI::touchCameraRotating(const float x, const float y)
{
    return false;
}


bool SceneGameUI::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void SceneGameUI::setFrameBufferID(const int frameBufferID)
{
    camera->setupViewport( camera->getViewportX(), camera->getViewportY(), camera->getViewportX2(), camera->getViewportY2(), frameBufferID );
}


void SceneGameUI::setHealthAlpha(const int index, float alpha)
{
    CCTile3DButton *tile = tileHealth[index];
    
    if( alpha >= 0.0f )
    {
        CCClampFloat( alpha, 0.0f, 1.0f );
        tile->setTileColourAlpha( alpha, true );
    }
    
    if( tile->getTileScaleInterpolator().isUpdating() == false )
    {
        LAMBDA_1( ScaleCallback, CCTile3DButton, tile,
                 {
                     tile->setTileScale( 1.0f, true );
                 });
        tile->setTileScale( 2.0f, true, new ScaleCallback( tile ) );
    }
}