/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneSplashScreen.h
 * Description : Splash screen tiles
 *
 * Created     : 28/06/12
 *-----------------------------------------------------------
 */

#ifndef __SCENESPLASHSCREEN_H__
#define __SCENESPLASHSCREEN_H__


class SceneSplashScreen : public CCSceneAppUI
{
public:    
    typedef CCSceneAppUI super;

    SceneSplashScreen();

    // CCSceneBase
	virtual void destruct();
	virtual void setup();
    
protected:
	virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
    
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
protected:
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
public:
    void show(const char *player1, const char *player2);
    void showPlayer1(const char *playerType, const float time=1.0f);
    void showPlayer2(const char *playerType, const float time=1.25f);
    void hide();
    
    static void LoadPlayerType(CCTile3DButton *tile, const char *playerType);
    
    bool isShowingPlayer1() { return showingPlayer1; }
    bool isShowingPlayer2() { return showingPlayer2; }
    
    CCTile3DButton* getPlayer1Stripe() { return tileBlueStripe; }
    CCTile3DButton* getPlayer2Stripe() { return tileRedStripe; }
    CCTile3DButton* getPlayer1Tile() { return tilePlayer1; }
    CCTile3DButton* getPlayer2Tile() { return tilePlayer2; }
    
protected:
    // Controls used for timers and such
    CCDestructList<CCUpdater> updaters;
    
    bool showingPlayer1, showingPlayer2;
    CCTile3DButton *tileWhiteStripe;
    CCTile3DButton *tileBlueStripe, *tileRedStripe, *tileVS;
    CCTile3DButton *tilePlayer1, *tilePlayer2;
    CCTile3DButton *tileCountry1, *tileCountry2;
};


#endif // __SCENEPLAYSCREEN_H__
