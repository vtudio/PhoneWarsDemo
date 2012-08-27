/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneAndroidsManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"

#include "CCAppManager.h"
#include "CCTextureBase.h"
#include "CCFileManager.h"


SceneAndroidsManager::SceneAndroidsManager()
{
#ifdef IOS
    playerType = "iBot";
#else
    playerType = "aBot";
#endif
}


void SceneAndroidsManager::destruct()
{
    super::destruct();
}



void SceneAndroidsManager::loadGame(const char *gameID)
{
    updaters.deleteObjects();
           
    showSplashScreen();
    setGameState( GameState_Playing );
    
    CCTimer *timer = new CCTimer();
    class EventCallback : public CCLambdaCallback                           
    {                                                                       
    public:                                                                 
        EventCallback(SceneAndroidsManager *scene, const char *gameID)                         
        {                                                                 
            this->scene = scene;                                          
            this->lazyPointer = scene->lazyPointer;                       
            this->lazyID = scene->lazyID;                                 
            this->gameID = gameID;                                          
        }                                                                   
        void run()                                                          
        {                                                                   
            if( CCLazyCallback::isCallbackActive( lazyPointer, lazyID ) )   
            {                         
                scene->hideSplashScreen();
                scene->sceneBackground->hide();
                scene->loadedGame( new SceneGameSyndicate( gameID.buffer ) );
            }                                                               
        }                                                                   
    private:                                                                
        SceneAndroidsManager *scene;                                                       
        void *lazyPointer;                                                  
        long lazyID;                                                      
        CCText gameID;                                                     
    };
    
    timer->onTime.add( new EventCallback( this, gameID ) );
    timer->start( 2.0f );
    updaters.add( timer );
}


void SceneAndroidsManager::loadedGame(SceneGameSyndicate *game)
{
    this->game = game;
    game->setParent( this );
    gEngine->addScene( game );
    
    startGame();
}


void SceneAndroidsManager::startGame()
{
    CharacterPlayer *player1, *player2;
    player1 = game->spawnCharacter( "player1", playerType.buffer );
    game->assignPlayerCharacter( player1 );
    
    if( CCText::Contains( playerType.buffer, "aBot" ) )
    {
        player2 = game->spawnCharacter( "player2", "iBot" );
        game->addFriend( player2 );
    }
    else
    {
        player2 = game->spawnCharacter( "player2", "aBot" );
        game->addFriend( player2 );
    }
    
    super::startGame();
}


void SceneAndroidsManager::showSplashScreen()
{
    sceneBackground->show();
    sceneSplashScreen->show( "iBot", "aBot" );
}


bool SceneAndroidsManager::isUsablePlayer()
{
    return true;
}


void SceneAndroidsManager::switchTeams()
{
    if( CCText::Equals( playerType, "aBot" ) )
    {
        playerType = "iBot";
    }   
    else if( CCText::Equals( playerType, "iBot" ) )
    {
        playerType = "aBot";
    }
}
