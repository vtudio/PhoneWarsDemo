/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectIndicator.h
 * Description : An indicator object.
 *
 * Created     : 02/05/12
 *-----------------------------------------------------------
 */

class ObjectIndicator : public CCObject
{
public:
    typedef CCObject super;

    ObjectIndicator(CCObjectCollideable *parentObject, const char *textureFile);
    ObjectIndicator(CCSceneBase *parentScene, const char *textureFile);
    
protected:
    virtual void renderModel(const bool alpha);

public:
    void setWidth(const float width);

public:
	CCPrimitiveSquare *primitive;
};
