/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectCollideableParticle.h
 * Description : A particle the moves without collision, but is depth sorted.
 *
 * Created     : 12/06/12
 *-----------------------------------------------------------
 */

class ObjectCollideableParticle : public CCObjectCollideable
{
public:
    typedef CCObjectCollideable super;

	ObjectCollideableParticle(const float inLife=1.0f);

	// ObjectBase
	virtual bool update(const CCTime &time);
    virtual void renderModel(const bool alpha);

protected:
	float life;
    bool facingCamera;
};