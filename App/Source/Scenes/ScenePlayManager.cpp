/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ScenePlayManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"

#include "CCAppManager.h"
#include "CCTextureBase.h"
#include "CCFileManager.h"

#ifdef ANDROID
#include "CCJNI.h"
#endif


ScenePlayManager* ScenePlayManager::scene = NULL;


ScenePlayManager::ScenePlayManager()
{   
    cameraCentered = true;

    scene = this;
    
    sceneBackground = new SceneBackground();
    gEngine->addScene( sceneBackground );
    
    sceneSplashScreen = new SceneSplashScreen();
    gEngine->addScene( sceneSplashScreen );
    
    camera = new CCCameraAppUI();
    gEngine->addCamera( camera );
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
    
    game = NULL;
}


void ScenePlayManager::destruct()
{
    // Delete sub scenes
    {
        if( game != NULL )
        {
            game->deleteLater();
            game = NULL;
        }
        
        if( sceneSplashScreen != NULL )
        {
            sceneSplashScreen->deleteLater();
            sceneSplashScreen = NULL;
        }
        
        if( sceneBackground != NULL )
        {
            sceneBackground->deleteLater();
            sceneBackground = NULL;
        }
    }
    
    scene = NULL;
    
    updaters.deleteObjects();
    
    super::destruct();
}


void ScenePlayManager::setup()
{
    CCText texPath = "Resources/";
    texPath += CLIENT_NAME;
    texPath += "/uimenu/ui_background.png";
    const float backgroundWidth = gEngine->textureManager->getTexture( texPath.buffer )->getRawWidth();
    camera->setCameraWidth( backgroundWidth, false );
    
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupText( " ", camera->targetHeight * 0.0725f, true, false );
        tile->drawOrder = 205;
        tile->setTextColour( 1.0f );
        tile->setTileScale( 1.0f );
        
        tile->setTextFlash( true );
        
        tileNotifications = tile;
        tileNotifications->setText( "tap to start" );
        
        LAMBDA_2( OrientationCallback, CCTile3DButton, tile, CCCameraAppUI, camera,
                 {
                     tile->setPositionXYZ( 0.0f, -camera->cameraHHeight + tile->collisionBounds.y, 0.0f );
                 });
        orientationCallbacks.add( new OrientationCallback( tile, camera ) );
    }
    
    // refresh the scene tile positioning and range
    beginOrientationUpdate();
    
    gotoSplashScreenGameState();
}


// CCSceneBase
bool ScenePlayManager::handleBackButton()
{
    if( gameState == GameState_Playing )
    {
    	LAMBDA_1( BackCallback, ScenePlayManager, scene,

				if( scene->game != NULL )
				{
					scene->game->deleteLater();
					scene->game = NULL;
				}
				scene->hideSplashScreen();
				scene->gotoSplashScreenGameState();

				)
		gEngine->nativeToEngineThread( new BackCallback( this ) );
        return true;
    }
	return false;
}


bool ScenePlayManager::updateScene(const CCTime &time)
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


bool ScenePlayManager::updateCamera(const CCTime &time)
{
    return super::updateCamera( time );
}


bool ScenePlayManager::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // All objects go through the Octree render pipe here
    return false;
}


void ScenePlayManager::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->renderObject( inCamera, alpha );
	}
}


void ScenePlayManager::beginOrientationUpdate()
{
    super::beginOrientationUpdate();
}


void ScenePlayManager::appPaused()
{
}


void ScenePlayManager::deletingChild(CCSceneBase *inScene)
{
    if( inScene == game )
    {
        game = NULL;
    }
}


bool ScenePlayManager::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool ScenePlayManager::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    return false;
}


bool ScenePlayManager::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    return super::touchReleased( touch, touchAction );
}


bool ScenePlayManager::touchCameraRotating(const float x, const float y)
{
    return false;
}


bool ScenePlayManager::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void ScenePlayManager::gotoSplashScreenGameState()
{
    setGameState( GameState_SplashScreen );
    
    tileNotifications->setText( "tap to start" );
    
    showSplashScreen();
}


void ScenePlayManager::hideSplashScreen()
{   
    updaters.deleteObjects();
    
    sceneSplashScreen->hide();
}


void ScenePlayManager::showPlayer1(const CharacterPlayer *character)
{   
    if( sceneSplashScreen->isShowingPlayer2() )
    {
        hideSplashScreen();
    }
    
    if( sceneSplashScreen->isShowingPlayer1() == false )
    {   
        sceneSplashScreen->showPlayer1( character->getType(), 0.5f );
        
        CCTile3DButton *tileStripe = sceneSplashScreen->getPlayer1Stripe();
        tileStripe->setTileColourAlpha( 0.25f, true );
    }
}


void ScenePlayManager::showPlayer2(const CharacterPlayer *character)
{ 
    if( sceneSplashScreen->isShowingPlayer1() )
    {
        hideSplashScreen();   
    }
    
    if( sceneSplashScreen->isShowingPlayer2() == false )
    {
        sceneSplashScreen->showPlayer2( character->getType(), 0.5f );
        
        CCTile3DButton *tileStripe = sceneSplashScreen->getPlayer2Stripe();
        tileStripe->setTileColourAlpha( 0.25f, true );
    }
}


void ScenePlayManager::start()
{
    if( gameState == GameState_SplashScreen )
    {
        updaters.deleteObjects();
        
        startOfflineGame();
    }
}



void ScenePlayManager::setGameState(const GameState state)
{
    gameState = state;
}


// Continued from child class
void ScenePlayManager::startGame()
{
    CharacterPlayer *player1 = game->getPlayer( "player1" );
    CharacterPlayer *player2 = game->getPlayer( "player2" );
    
    game->setLocation( player1->getPlayerID(), CCVector3( -50.0f, player1->getPosition().y, 0.0f ) );
    game->setLocation( player2->getPlayerID(), CCVector3( 50.0f, player2->getPosition().y, 0.0f ) );
    
    for( int i=0; i<4; ++i )
    {
        PickupBase *pickup = game->spawnPickup( "healthpack", "health", NULL );
        const CCSize &mapBounds = game->getMapBounds();
        CCVector3 location;
        location.x = mapBounds.width * 0.9f * CCRandomDualFloat();
        location.z = mapBounds.height * 0.9f * CCRandomDualFloat();
        pickup->setPositionXZ( location.x, location.z );
        game->adjustCollisionPlacement( pickup );
    }
    
    {
        CharacterUpdaterAI *aiUpdater = new CharacterUpdaterAI( game->getPathFinderNetwork() );
        player2->setupAI( aiUpdater );
        aiUpdater->scanWaypoints( CC_SQUARE( 75.0f ) );
        aiUpdater->setWaypointCycle( CharacterUpdaterAI::cycle_aiNodes );
        aiUpdater->setMovementMagnitude( 0.5f );
        aiUpdater->setEnemy( player1 );
    }
    
    tileNotifications->setText( "" );
}


void ScenePlayManager::matchEnd()
{
    hideSplashScreen();
    
    if( game != NULL )
    {
        game->deleteLater();
        game = NULL;
    }
    
    gotoSplashScreenGameState();
}


void ScenePlayManager::startOfflineGame()
{
    if( game == NULL )
    {
        tileNotifications->setText( "loading" );
        loadGame( NULL );
    }
}


CharacterPlayer* ScenePlayManager::SpawnCharacter(const char *type)
{
    if( CCText::Contains( type, "burger" ) ||
        CCText::Contains( type, "fries" ) )
    {
        return CharacterPlayerBurger::Spawn( type );
    }
    else if( CCText::Contains( type, "aBot" ) ||
             CCText::Contains( type, "iBot" ) )
    {
        return CharacterPlayerAndroid::Spawn( type );
    }
    
    return CharacterPlayerAndroid::Spawn( "iphone" );
}
