/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTexturePNG.h
 * Description : Android specific texture loader for JPG/GIF/PNG.
 *
 * Created     : 15/05/11
 * Author(s)   : Chris Bowers, Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTEXTUREPNG_H__
#define __CCTEXTUREPNG_H__

#include "CCTextureBase.h"

class CCTexturePNG : public CCTextureBase
{
public:
	typedef CCTextureBase super;

	CCTexturePNG();
    virtual ~CCTexturePNG();

    // TextureBase
    virtual bool load(const char *name, const CCResourceType resourceType, const bool generateMipMap);
    virtual bool createGLTexture();
    
    virtual float getRawWidth() const { return (float)rawWidth; }
    virtual float getRawHeight() const { return (float)rawHeight; }

    static bool DoesTextureExist(const char *path, const CCResourceType resourceType);

private:
    float rawWidth, rawHeight;
};

#endif // __TEXTUREPNG_H__

