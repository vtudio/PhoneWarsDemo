/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneSplashScreen.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"

#include "CCAppManager.h"
#include "CCTextureBase.h"
#include "CCFileManager.h"


SceneSplashScreen::SceneSplashScreen()
{   
    cameraCentered = true;

    camera = new CCCameraAppUI();
    gEngine->addCamera( camera );
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
    
    showingPlayer1 = showingPlayer2 = false;
}


void SceneSplashScreen::destruct()
{
    updaters.deleteObjects();
    
    super::destruct();
}


void SceneSplashScreen::setup()
{
    CCText texPath = "Resources/";
    texPath += CLIENT_NAME;
    texPath += "/uimenu/ui_background.png";
    
    const float backgroundWidth = gEngine->textureManager->getTexture( texPath.buffer )->getRawWidth();
    camera->setCameraWidth( backgroundWidth, false );
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupTextured( "Resources/common/uimenu/ui_vs_whitestripe.png", Resource_Packaged, true );
        tile->setTileScale( 1.0f );
        tileWhiteStripe = tile;
        tile->shouldRender = false;
    }
    
    {
        {
            CCTile3DButton *tile = new CCTile3DButton( this );
            tile->setupTextured( "Resources/common/uimenu/ui_vs_bluestripe.png", Resource_Packaged, true );
            tile->setTileScale( 1.0f );
            tile->positionTileAbove( tileWhiteStripe );
            tile->drawOrder = 201;
            tileBlueStripe = tile;
            
            tile->translate( camera->targetWidth, 0.0f, 0.0f );
            
            {
                CCTile3DButton *tile = new CCTile3DButton( this );
                tile->setupTextured( "Resources/common/uimenu/ui_vs_whitestripe.png", Resource_Packaged, true );
                tile->setTileScale( 1.0f );
                tile->drawOrder = 202;
                
                tile->translate( 0.0f, ( tileBlueStripe->collisionBounds.y + tile->collisionBounds.y ), 0.0f );
                
                tile->removeFromScene();
                tileBlueStripe->addChild( tile );
            }
            
            {
                CCTile3DButton *tile = new CCTile3DButton( this );
                tile->setupTextured( "Resources/common/uimenu/ui_vs_whitestripe.png", Resource_Packaged, true );
                tile->setTileScale( 1.0f );
                tile->drawOrder = 203;
                
                tile->translate( 0.0f, -( tileBlueStripe->collisionBounds.y + tile->collisionBounds.y ), 0.0f );
                
                tile->removeFromScene();
                tileBlueStripe->addChild( tile );
            }
        }
    }
    
    {
        {
            CCTile3DButton *tile = new CCTile3DButton( this );
            tile->setupTextured( "Resources/common/uimenu/ui_vs_redstripe.png", Resource_Packaged, true );
            tile->setTileScale( 1.0f );
            tile->positionTileBelow( tileWhiteStripe );
            tile->drawOrder = 201;
            tileRedStripe = tile;
            
            tile->translate( -camera->targetWidth, 0.0f, 0.0f );
            
            {
                CCTile3DButton *tile = new CCTile3DButton( this );
                tile->setupTextured( "Resources/common/uimenu/ui_vs_whitestripe.png", Resource_Packaged, true );
                tile->setTileScale( 1.0f );
                tile->drawOrder = 203;
                
                tile->translate( 0.0f, ( tileRedStripe->collisionBounds.y + tile->collisionBounds.y ), 0.0f );
                
                tile->removeFromScene();
                tileRedStripe->addChild( tile );
            }
            
            {
                CCTile3DButton *tile = new CCTile3DButton( this );
                tile->setupTextured( "Resources/common/uimenu/ui_vs_whitestripe.png", Resource_Packaged, true );
                tile->setTileScale( 1.0f );
                tile->drawOrder = 202;
                
                tile->translate( 0.0f, -( tileRedStripe->collisionBounds.y + tile->collisionBounds.y ), 0.0f );
                
                tile->removeFromScene();
                tileRedStripe->addChild( tile );
            }
        }
    }

    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setTileScale( 1.0f );
        tile->drawOrder = 203;
        tilePlayer1 = tile;
    }
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setTileScale( 1.0f );
        tile->drawOrder = 204;
        tilePlayer2 = tile;
    }
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setTileScale( 1.0f );
        tile->drawOrder = 203;
        tileCountry1 = tile;
    }
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setTileScale( 1.0f );
        tile->drawOrder = 204;
        tileCountry2 = tile;
    }
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupTextured( "Resources/common/uimenu/ui_vs.png", Resource_Packaged, true );
        tile->setTileScale( 1.0f );
        tile->drawOrder = 205;
        
        tileVS = tile;
    }

    refreshCameraView();
    lockCameraView();
}


// CCSceneBase
bool SceneSplashScreen::updateScene(const CCTime &time)
{
    bool updated = super::updateScene( time );
    
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
        }
    }
    
    return updated;
}


bool SceneSplashScreen::updateCamera(const CCTime &time)
{
    return super::updateCamera( time );
}


bool SceneSplashScreen::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // All objects go through the Octree render pipe here
    return false;
}


void SceneSplashScreen::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->renderObject( inCamera, alpha );
	}
}


bool SceneSplashScreen::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool SceneSplashScreen::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    return false;
}


bool SceneSplashScreen::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    return super::touchReleased( touch, touchAction );
}


bool SceneSplashScreen::touchCameraRotating(const float x, const float y)
{
    return false;
}


bool SceneSplashScreen::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void SceneSplashScreen::show(const char *player1, const char *player2)
{   
    showPlayer1( player1 );
    showPlayer2( player2 );
    
    tileVS->setTileMovement( CCVector3( 0.0f, 0.0f, 0.0f ) );
    tileVS->setTileColourAlpha( 1.0f, true );
}


void SceneSplashScreen::showPlayer1(const char *playerType, const float time)
{
    if( showingPlayer1 == false )
    {
        showingPlayer1 = true;
        
        CCTimer *timer = new CCTimer();
        
        {
            CCTile3DButton *tile = tilePlayer1;
            LoadPlayerType( tile, playerType );
            
            tile->positionTileAbove( tileWhiteStripe );
            tile->setPositionX( -camera->targetWidth * 0.75f );
        }
        
        {
            LAMBDA_1( EventCallback, SceneSplashScreen, scene,                         
                     {
                         CCCameraAppUI *camera = scene->camera;
                         {
                             CCTile3DButton *tile = scene->tilePlayer1;
                             tile->setTileMovementX( -camera->targetWidth * 0.5f + tile->collisionBounds.x );
                         }
                         {
                             CCTile3DButton *tile = scene->tileCountry1;
                             tile->setTileMovementX( camera->targetWidth * 0.5f - tile->collisionBounds.x );
                         }
                     })  
            timer->onTime.add( new EventCallback( this ) );
        }
        
        timer->start( time );
        updaters.add( timer );
        
        tileBlueStripe->setTileColourAlpha( 1.0f, true );
        tileBlueStripe->setTileMovement( CCVector3( 0.0f, tileBlueStripe->getPosition().y, 0.0f ) );
    }
}


void SceneSplashScreen::showPlayer2(const char *playerType, const float time)
{
    if( showingPlayer2 == false )
    {
        showingPlayer2 = true;
        
        CCTimer *timer = new CCTimer();
        
        {
            CCTile3DButton *tile = tilePlayer2;
            tile->resetTileUVs();
            LoadPlayerType( tile, playerType );
            tile->flipTileY();
            
            tile->positionTileAbove( tileRedStripe );
            tile->translate( 0.0f, -tileRedStripe->collisionBoundsLength.height, 0.0f );
            tile->setPositionX( camera->targetWidth * 0.75f );
        }
        
        {
            LAMBDA_1( EventCallback, SceneSplashScreen, scene,                         
                     {
                         CCCameraAppUI *camera = scene->camera;
                         {
                             CCTile3DButton *tile = scene->tilePlayer2;
                             tile->setTileMovementX( camera->targetWidth * 0.5f - tile->collisionBounds.x );
                         }
                         {
                             CCTile3DButton *tile = scene->tileCountry2;
                             tile->setTileMovementX( -camera->targetWidth * 0.5f + tile->collisionBounds.x );
                         }
                     })  
            timer->onTime.add( new EventCallback( this ) );
        }

        timer->start( time );
        updaters.add( timer );
        
        tileRedStripe->setTileColourAlpha( 1.0f, true );
        tileRedStripe->setTileMovement( CCVector3( 0.0f, tileRedStripe->getPosition().y, 0.0f ) );
    }
}


void SceneSplashScreen::hide()
{   
    updaters.deleteObjects();
    
    tileBlueStripe->setTileColourAlpha( 0.0f, true );
    tileBlueStripe ->setTextColourAlpha( 0.0f, true );
    tileRedStripe->setTileColourAlpha( 0.0f, true );
    tileRedStripe->setTextColourAlpha( 0.0f, true );
    
    tileBlueStripe->setTileMovementX( -camera->targetWidth );
    tilePlayer1->setTileMovementX( camera->targetWidth );
    tileCountry1->setTileMovementX( -camera->targetWidth );
    tileRedStripe->setTileMovementX( camera->targetWidth );
    tilePlayer2->setTileMovementX( -camera->targetWidth );
    tileCountry2->setTileMovementX( camera->targetWidth );
    
    tileVS->setTileMovement( CCVector3( 0.0f, 0.0f, camera->targetOffset.z * 1.0f ) );
    tileVS->setTileColourAlpha( 0.0f, true );
    
    showingPlayer1 = showingPlayer2 = false;
}


void SceneSplashScreen::LoadPlayerType(CCTile3DButton *tile, const char *playerType)
{   
    tile->setupTextured( "Resources/androids/uimenu/ui_iBot_cropped.png", Resource_Packaged, true );
    if( CCText::Contains( playerType, "aBot" ) )
    {
        tile->setupTextured( "Resources/androids/uimenu/ui_aBot_cropped.png", Resource_Packaged, true );
    }
    else if( CCText::Contains( playerType, "burger" ) )
    {
        tile->setupTextured( "Resources/burgers/uimenu/ui_burger_cropped.png", Resource_Packaged, true );
    }
    else if( CCText::Contains( playerType, "fries" ) )
    {
        tile->setupTextured( "Resources/burgers/uimenu/ui_fries_cropped.png", Resource_Packaged, true );
    }
}