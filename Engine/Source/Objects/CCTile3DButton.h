/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTile3DButton.h
 * Description : A dynamic button widget.
 *
 * Created     : 14/09/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTILE3DBUTTON_H__
#define __CCTILE3DBUTTON_H__


class CCModelText;

class CCTile3DButton : public CCTile3D
{
public:
    typedef CCTile3D super;
    
    CCTile3DButton(CCSceneBase *scene);
    
    void setupTile(const float size=1.0f, const char *text=NULL);
    void setupTile(const float width, const float height, const char *text=NULL);
    
    // Create tile with width and textures aspect ratio
    void setupTextured(const char *textureName, const CCResourceType resourceType, const bool mipmap=false);
    void setupTexturedWidth(const float width, const char *textureName, const CCResourceType resourceType, const bool mipmap=false);
    void setupTexturedHeight(const float heigth, const char *textureName, const CCResourceType resourceType, const bool mipmap=false);
    void setupTexturedFit(const float width, const float height, const char *textureName, const bool mipmap=false);
    
    // Create tile with the text height
    void setupText(const char *text, const float textHeight, const bool centered=true, const bool background=false);
    
private: 
    void construct(CCSceneBase *scene);
public:
    virtual void destruct();
    
	// CCRenderable
	virtual void refreshModelMatrix();
    
    virtual void setPositionXYZ(const float x, const float y, const float z);

	// CCObject
    virtual bool update(const CCTime &time);
    
protected:
    virtual void renderModel(const bool alpha);

public:
    void setTileSize(const float size=1.0f);
    virtual void setTileSize(const float width, const float height);
    void setTileTexture(const char *name, const CCResourceType resourceType, const bool mipmap=false, const bool load=false);
    void setTileTexturedSize();
    void setTileTexturedWidth(const float width);
    void setTileTexturedHeight(const float height);
    void setTileTexturedFit(const float width, const float height);
    
    const CCColour& getTileColour() { return *tileModel->getColour(); }
    CCInterpolatorLinearColour& getTileColourInterpolator() { return tileColourInterpolator; }
    void setTileColour(const CCColour &inColour, const bool interpolate=false);
    void setTileColourAlpha(const float inAlpha, const bool interpolate=false, CCLambdaCallback *inCallback=NULL);
    
    void setTileScale(const CCVector3 inScale, const bool interpolate=false, CCLambdaCallback *onInterpolated=NULL);
    void setTileMovement(const CCVector3 target);
    void setTileMovementX(const float x);
    void translateTileMovementX(const float x);
    void setTileMovementY(const float y);
    void setTileMovementBelow(const CCTile3DButton *fromTile);
    const CCVector3 getTileMovementTarget() const;
    
    void resetTileUVs();
    void flipTileY();
    
    inline CCPrimitiveSquare* getTileSquare() { return tileSquare; }
    
    virtual void setText(const char *text, const bool resizeTile=false, const float height=-1.0f)
    {
        textModel->setText( text, height );
        if( resizeTile )
        {
            const float width = textModel->getWidth();
            const float height = textModel->getHeight();
            setCollisionBounds( width, height, CC_SMALLFLOAT );
            CCUpdateCollisions( this );
            if( tileSquare != NULL )
            {
                tileSquare->setupZFacing( collisionBoundsLength.width, collisionBoundsLength.height );
            }
        }
    }
    
    void setTextPosition(const float x, const float y);
    
    void setTextColour(const CCColour &colour, const bool interpolate=false)
    {
        textModel->setColour( colour, interpolate );
    }
    
    void setTextColourAlpha(const float inAlpha, const bool interpolate=false, CCLambdaCallback *inCallback=NULL)
    {
        textModel->setColourAlpha( inAlpha, interpolate, inCallback );
    }
    
    void setTextAlpha(const float alpha, const bool interpolate)
    {
        textModel->setColourAlpha( alpha, interpolate );
    }
    
    void setTextFont(const char *font, const bool resizeTile=false)
    {
        textModel->setFont( font );
        if( resizeTile )
        {
            const float width = textModel->getWidth();
            const float height = textModel->getHeight();
            setCollisionBounds( width, height, CC_SMALLFLOAT );
            CCUpdateCollisions( this );
            if( tileSquare != NULL )
            {
                tileSquare->setupZFacing( collisionBoundsLength.width, collisionBoundsLength.height );
            }
        }
    }
    
    virtual void setTextHeight(const float height)
    {
        textModel->setHeight( height );
    }
    
    void setTextFlash(const bool toggle);
    
    CCModelText* getTextModel() { return textModel; }
    
    void setReadDepth(const bool toggle) { readDepth = toggle; }
    
    // Touchable
    virtual bool handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                      const CCObjectCollideable *hitObject,
                                      const CCVector3 &hitPosition,
                                      const CCScreenTouches &touch,
                                      const CCTouchAction touchAction);

    // Called when the tile is touched
    virtual void touchActionPressed(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction);
    
    // Called when a touch is moved over this tile
    virtual void touchActionMoved(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction);

    // Called when the tile is released
    virtual void touchActionRelease(const CCTouchAction touchAction);

protected:
    // Callbacks
    virtual void handleTouchRelease();

public:
    inline void allowTouchRotation(const bool allow) { touchRotationAllowed = allow; }
    const CCVector3 getRotationTarget() 
    {
        if( touchRotationInterpolator.interpolators.length > 0 )
        {
            return touchRotationInterpolator.interpolators.list[touchRotationInterpolator.interpolators.length-1]->getTarget();
        }
        return rotation;
    }

    void allowTouchMovement(const bool allow=true)
    {
        touchMovementAllowed = allow;
    }
    
    void setTouchRotationSpeed(const float speed)
    {
        touchRotationSpeed = speed;
    }
    
    void setTouchDepressRange(const float range)
    {
        touchDepressRange = range;
    }

    void setTouchDepressDepth(const float depth)
    {
        touchDepressDepth = depth;
    }
    
    inline CCInterpolatorV3<CCInterpolatorSin2Curve>& getTileScaleInterpolator() { return tileScaleInterpolator; }

protected:
    CCModelBase *tileModel;
    CCPrimitiveSquare *tileSquare;
    
    CCObject *textObject;
    CCModelText *textModel;
    
    CCInterpolatorV3<CCInterpolatorSin2Curve> tileScaleInterpolator;

    bool touchMovementAllowed;
    
    bool touchRotationAllowed;
    CCInterpolatorListV3<CCInterpolatorLinear> touchRotationInterpolator;
    float touchRotationMagnitude;
    float touchRotationSpeed;
    
    // Touch Position interpolation
    float touchDepressRange;
    float touchDepressDepth;
    CCVector3 touchDepressPosition;
    CCInterpolatorListV3<CCInterpolatorSin2Curve> touchDepressInterpolator;
    
    CCInterpolatorLinearColour tileColourInterpolator;
    
    bool moving;
    CCInterpolatorListV3<CCInterpolatorX3Curve> touchMovementInterpolator;
};


#endif // __CCTILE3DBUTTON_H__
