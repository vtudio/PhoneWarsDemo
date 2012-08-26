/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CollideableFloor.h
 * Description : A floor object.
 *
 * Created     : 31/01/10
 *-----------------------------------------------------------
 */

class CollideableWall : public CCObjectCollideable
{
public:
    typedef CCObjectCollideable super;
    
    CollideableWall();

	// RenderableBase
	virtual void setPositionXYZ(const float x, const float y, const float z);

	// ObjectBase
	virtual void renderModel(const bool alpha);

	// CollideableBase
	virtual bool shouldCollide(CCObjectCollideable *collideWith, const bool initialCall);
    
	virtual void setup(CCSceneBase *scene, const float x, const float z, const float size, const float height);
    void createModel();
    
public:
	CCPrimitiveSquare *primitive;

protected:
	float offsetX, offsetZ;
};
