/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneBackground.h
 * Description : Our UI background.
 *
 * Created     : 28/06/12
 *-----------------------------------------------------------
 */

#ifndef __SCENEBACKGROUND_H__
#define __SCENEBACKGROUND_H__


class SceneBackground : public CCSceneAppUI
{
public:
    typedef CCSceneAppUI super;

    SceneBackground();

    // CCSceneBase
	virtual void destruct();
	virtual void setup();
    
protected:
	virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
    
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
    virtual void beginOrientationUpdate();
    
protected:
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
public:
    void show();
    void hide();
    
protected:
    CCTile3DButton *tileBackground;
};


#endif // __SCENEBACKGROUND_H__
