
/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCModelObj.h
 * Description : Loads and handles an obj model.
 *
 * Created     : 26/12/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCMODELOBJ_H__
#define __CCMODELOBJ_H__

class CCPrimitiveObj : public CCPrimitiveBase
{
public:
    typedef CCPrimitiveBase super;
    
    CCPrimitiveObj();
    virtual void destruct();
    
	const bool load(const char *file);
    
    // PrimitiveBase
protected:
    // Adjust the model's UVs to match the loaded texture, 
    // as non-square textures load into a square texture which means the mapping requires adjustment
    void adjustTextureUVs();
    
public:
	virtual void renderVertices(const bool textured);
    
    const float getWidth() { return width; }
    const float getHeight() { return height; }
    const float getDepth() { return depth; }
    
    const CCMinMax getYMinMax() const { return mmY; }
    const CCMinMax getYMinMaxAtZ(const float atZ) const;
    const CCMinMax getZMinMax() const { return mmZ; }
    
    const CCVector3 getOrigin();
    void moveVerticesToOrigin();
    const bool hasMovedToOrigin() { return movedToOrigin; }
    
    void copy(const CCPrimitiveObj *primitive);
    
protected:
	uint vertexCount;
    float *modelUVs;
    float *textureUVs;
    
    float width, height, depth;
    CCMinMax mmX, mmY, mmZ;
    
    bool cached;
    bool movedToOrigin;
    CCVector3 origin;
};

class CCModelObj : public CCModelBase
{
public:
    typedef CCModelBase super;
    
    static CCModelObj* CacheModel(const char *file, const char *texture, 
                                  const bool moveVerticesToOrigin=true,
                                  const CCResourceType resourceType=Resource_Packaged, 
                                  const bool mipmap=true, const bool load=true);

    CCModelObj();
    
	CCModelObj(const char *file, const char *texture=NULL, 
               const bool moveVerticesToOrigin=false,
               const CCResourceType resourceType=Resource_Packaged, 
               const bool mipmap=true, const bool load=true);
    
    const CCPrimitiveObj* getPrimitive() const { return primitive; }
    
    const float getWidth() { return primitive->getWidth(); }
    const float getHeight() { return primitive->getHeight(); }
    const float getDepth() { return primitive->getDepth(); }
    
    const CCVector3 getOrigin()
    {
        return primitive->getOrigin();
    }
    
    void moveVerticesToOrigin()
    {
        primitive->moveVerticesToOrigin();
    }

public:
	CCPrimitiveObj *primitive;
};

#endif // __CCMODELOBJ_H__
