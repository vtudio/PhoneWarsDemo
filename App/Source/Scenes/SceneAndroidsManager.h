/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneAndroidsManager.h
 * Description : Androids game manager.
 *
 * Created     : 25/07/12
 *-----------------------------------------------------------
 */

#ifndef __SCENEANDROIDSMANAGER_H__
#define __SCENEANDROIDSMANAGER_H__


class SceneAndroidsManager : public ScenePlayManager
{
public:
    typedef ScenePlayManager super;

    SceneAndroidsManager();

    // CCSceneBase
	virtual void destruct();
    
protected:
    void loadGame(const char *gameID);
    void loadedGame(SceneGameSyndicate *game);
    void startGame();
    
public:
    virtual void showSplashScreen();
    
    virtual bool isUsablePlayer();
    virtual void switchTeams();
};


#endif // __SCENEANDROIDSMANAGER_H__
