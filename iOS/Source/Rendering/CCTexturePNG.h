/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTexturePNG.h
 * Description : iOS specific texture loader for PNG/JPG.
 *
 * Created     : 27/04/10
 * Author(s)   : Ashraf Samy Hegab
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

	// CCTextureBase
	virtual bool load(const char *path, const CCResourceType resourceType, const bool generateMipMap);
	virtual bool createGLTexture();

private:
	static bool extensionSupported(const char* extension);
};


#endif // __CCTEXTUREPNG_H__

