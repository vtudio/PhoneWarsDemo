/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCModelBase.h
 * Description : Represents the attributes of a renderable object.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCMODELBASE_H__
#define __CCMODELBASE_H__


#include "CCRenderable.h"

class CCPrimitiveBase;

class CCModelBase : public CCRenderable
{
public:
	typedef CCRenderable super;

	CCModelBase();
	virtual void destruct();
	
	// CCRenderable
	virtual void render(const bool alpha);
	
	void addModel(CCModelBase *model, const int index=-1);
    void removeModel(CCModelBase *model);
	void addPrimitive(CCPrimitiveBase *primitive);
    
    inline CCColour* getColour() { return colour; }
    inline CCColour* getOutline() { return colourOutline; }
	
	virtual void setColour(const CCColour &inColour);
	void setColourAlpha(const float inAlpha);
	void setOutline(const CCColour &inColour);
	
    CCDestructList<CCModelBase> models;
    CCDestructList<CCPrimitiveBase> primitives;
	
protected:
	CCColour *colour;
	CCColour *colourOutline;
    
public:
    const char *shader;
};


#endif // __CCMODELBASE_H__
