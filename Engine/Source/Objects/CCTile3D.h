/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTile3D.h
 * Description : Base class for our Tile widgets.
 *
 * Created     : 14/09/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTILE3D_H__
#define __CCTILE3D_H__


class CCTouchable
{
public:
    CCTouchable()
    {
        touching = false;
        touchReleased = false;
    }
    
    inline float getTouchingTime() { return touchingTime; }
    
    virtual bool handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                      const CCObjectCollideable *hitObject,
                                      const CCVector3 &hitPosition,
                                      const CCScreenTouches &touch,
                                      const CCTouchAction touchAction) = 0;
    
    // Called when the tile is touched
    virtual void touchActionPressed(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction) = 0;
    
    // Called when a touch is moved over this tile
    virtual void touchActionMoved(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction) = 0;
    
    // Called when the tile is released
    virtual void touchActionRelease(const CCTouchAction touchAction) = 0;
    
protected:
    // Callbacks
    virtual void handleTouchRelease() = 0;
    virtual void onTouchPress()
    {
        LAMBDA_EMIT( onPress );
    }
    
    virtual void onTouchMove()
    {
        LAMBDA_EMIT( onMove );
    }

    virtual void onTouchRelease()
    {
        LAMBDA_EMIT( onRelease );
    }

    virtual void onTouchLoss()
    {
        LAMBDA_EMIT( onLoss );
    }
    
protected:
    bool touching;
    float touchingTime;
    bool touchReleased;
    
public:
    LAMBDA_SIGNAL onPress;
    LAMBDA_SIGNAL onMove;
    LAMBDA_SIGNAL onRelease;
    LAMBDA_SIGNAL onLoss;
};



class CCTile3D : public CCObjectCollideable, public CCTouchable, public virtual CCLazyCallback
{
public:
    typedef CCObjectCollideable super;

    CCTile3D();
    virtual void destruct();

    // CCRenderable
    virtual void dirtyModelMatrix();
    virtual void setPositionXYZ(const float x, const float y, const float z);
    virtual void translate(const float x, const float y, const float z);
    
    // Positioning Tiles
    void positionTileY(float &y);
    virtual void positionTileBelow(CCTile3D *fromTile);
    void positionTileAbove(CCTile3D *fromTile);
    void positionTileRight(CCTile3D *fromTile);
    void positionTileLeft(CCTile3D *fromTile);

    // Objects which move along with this tile, but contain handle their own collisions
    CCList<CCObject> attachments;
};


#endif // __CCTILE3D_H__
