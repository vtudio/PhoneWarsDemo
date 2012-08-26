/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCRenerable.h
 * Description : A renderable component.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCRENDERABLE_H__
#define __CCRENDERABLE_H__


class CCRenderable : public CCBaseType
{
public:
    typedef CCBaseType super;
	
    CCRenderable();

    // CCBaseType
    virtual void destruct();
    
    virtual void dirtyModelMatrix();
    virtual void dirtyWorldMatrix();
    
    virtual void refreshModelMatrix();
    void refreshWorldMatrix(const CCMatrix *parentMatrix);

    inline const CCVector3& getPosition() const { return position; }
    void setPosition(const CCVector3 &vector);
    virtual void setPositionXYZ(const float x, const float y, const float z);
    void setPositionX(const float x);
    void setPositionY(const float x);
    void setPositionXY(const float x, const float z);
    void setPositionXZ(const float x, const float z);

    void translate(CCVector3 *vector);
    virtual void translate(const float x, const float y, const float z);

    virtual void rotationUpdated();
    void setRotation(const CCVector3 &vector);
    void setRotationY(const float y);
    void rotateX(const float x);
    void rotateY(const float y);
    void rotateZ(const float z);

public:
    bool shouldRender;
    
    CCMatrix modelMatrix;
    bool updateModelMatrix;

    bool updateWorldMatrix;
    CCMatrix worldMatrix;

    CCVector3 *positionPtr;
    CCVector3 rotation;
    CCVector3 *scale;

protected:
    CCVector3 position;
};


#endif // __CCRENDERABLE_H__
