/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCPrimitiveBase.h
 * Description : Base drawable component.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCPRIMITIVEBASE_H__
#define __CCPRIMITIVEBASE_H__


class CCPrimitiveBase : public CCBaseType, public virtual CCLazyCallback
{
public:
    typedef CCBaseType super;

	CCPrimitiveBase();

    // CCBaseType
	virtual void destruct();
	
    // name specifies local path to the texture
    // resourceType specifies is the data is generated during runtime or part of the install package
    // alpha specifies if the image should function through the transparency pipe
    virtual void setTexture(const char *name, const CCResourceType resourceType, 
                            const bool mipmap=false, const bool load=false, const bool alwaysResident=false );
    void removeTexture();
    
    void setFrameBufferID(const int frameBufferID) { this->frameBufferID = frameBufferID; }

protected:
    // Adjust the model's UVs to match the loaded texture, 
    // as non-square textures load into a square texture which means the mapping requires adjustment
    virtual void adjustTextureUVs() {};
	
public:
	void render();
	virtual void renderVertices(const bool textured) = 0;
	virtual void renderOutline() {};
	
	float *vertices;
	float *normals;	
    
    struct TextureInfo
    {
        TextureInfo()
        {
            primaryIndex = secondaryIndex = 0;
        }
        int primaryIndex;
        int secondaryIndex;
        
    };
	TextureInfo *textureInfo;
    int frameBufferID;
};


#endif // __CCPRIMITIVEBASE_H__
