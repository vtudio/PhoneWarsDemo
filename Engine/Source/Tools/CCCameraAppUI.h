/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCCameraAppUI.h
 * Description : AppUI Scene camera functionality.
 *
 * Created     : 27/04/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCCameraAppUI : public CCCameraBase
{
public:
    typedef CCCameraBase super;

	CCCameraAppUI();
    
    virtual void update();
    virtual void setLookAt(const CCVector3 &newLookAt);
    virtual void setOffset(const CCVector3 &newOffset);

    virtual bool interpolateCamera(const float delta, const float speed);
    
    inline CCInterpolatorV3<CCInterpolatorX3Curve>& getLookAtInterpolator() { return lookAtInterpolator; }
    inline CCInterpolatorV3<CCInterpolatorSin2Curve>& getOffsetInterpolator() { return offsetInterpolator; }
    
    const CCVector3Target& getLookAtTarget() { return currentLookAtTarget; }
    
protected:
    CCVector3Target currentLookAtTarget;
    CCVector3Target currentOffsetTarget;
    
    CCInterpolatorV3<CCInterpolatorX3Curve> lookAtInterpolator;
    CCInterpolatorV3<CCInterpolatorSin2Curve> offsetInterpolator;
    
public:
    void setCameraWidth(const float inWidth, const bool interpolate=true);
    void setCameraHeight(const float inHeight, const bool interpolate=true);
    void refreshCameraSize();
    
    float calcCameraOffset(const float inWidth);
    float calcCameraWidth(const float inOffset);
    float calcCameraHeight(const float inOffset);
    
public:
    CCVector3 targetOffset;
    CCVector3 targetLookAt;
    float targetWidth, targetHeight;
    float cameraWidth, cameraHeight, cameraHWidth, cameraHHeight;
};

