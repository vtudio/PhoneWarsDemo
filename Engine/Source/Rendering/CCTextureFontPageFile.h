/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTextureFontPageFile.h
 * Description : Handles loadng font description files.
 *
 * Created     : 20/04/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTEXTUREFONTPAGEFILE_H__
#define __CCTEXTUREFONTPAGEFILE_H__


#include "CCTextureFontPage.h"

class CCTextureFontPageFile : public CCTextureFontPage
{
public:
	typedef CCTextureFontPage super;

	CCTextureFontPageFile(const char *inName);
	virtual ~CCTextureFontPageFile();

    virtual bool load(const char *path, const CCResourceType resourceType, const bool generateMipMap);

protected:
	virtual void bindTexturePage() const;

protected:
	uint texturePageIndex;
};


#endif // __CCTEXTUREFONTPAGEFILE_H__
