/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ScenePlayManager.h
 * Description : Base play match manager.
 *
 * Created     : 25/07/12
 *-----------------------------------------------------------
 */

#ifndef __SCENEPLAYMANAGER_H__
#define __SCENEPLAYMANAGER_H__


class ScenePlayManager : public CCSceneAppUI
{
public:
    typedef CCSceneAppUI super;

    ScenePlayManager();

    // CCSceneBase
	virtual void destruct();
	virtual void setup();
    
    virtual bool handleBackButton();
    
protected:
	virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
    
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
    virtual void beginOrientationUpdate();
    
    void appPaused();
    
protected:
    virtual void deletingChild(CCSceneBase *inScene);
    
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
protected:
    void gotoSplashScreenGameState();
    
public:
    virtual void showSplashScreen() = 0;  
    void hideSplashScreen();
    
    void showPlayer1(const CharacterPlayer *character);
    void showPlayer2(const CharacterPlayer *character);

    void start();
    
protected:
    enum GameState
    {
        GameState_SplashScreen,
        GameState_Playing,
    };
    void setGameState(const GameState state);
    
    virtual void loadGame(const char *gameID) = 0;
    virtual void loadedGame(SceneGameSyndicate *game) = 0;
    virtual void startGame();
    
public:
    void matchEnd();
    
public:
    void startOfflineGame();
    
    virtual bool isUsablePlayer() = 0;
    virtual void switchTeams() = 0;
    
    static CharacterPlayer* SpawnCharacter(const char *type);

public:
    static ScenePlayManager *scene;
    
protected:
    // Controls used for timers and such
    CCDestructList<CCUpdater> updaters;
    
    SceneBackground *sceneBackground;
    SceneSplashScreen *sceneSplashScreen;
    
    CCTile3DButton *tileNotifications;
    
    GameState gameState;
    
    CCText playerType;
    SceneGameSyndicate *game;
};


#endif // __SCENEPLAYMANAGER_H__
