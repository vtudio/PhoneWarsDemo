/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTextureFontPage.h
 * Description : Handles rendering text.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTEXTUREFONTPAGE_H__
#define __CCTEXTUREFONTPAGE_H__


#include "CCTextureBase.h"

class CCTextureFontPage
{
public:
    virtual ~CCTextureFontPage() {}
    
    inline const char* getName() const { return name; }
    float getCharacterWidth(const char character, const float size) const;
    float getWidth(const char *text, const uint length, const float size) const;
    float getHeight(const char *text, const uint length, const float size) const;
    
    void renderText(const char *text, const uint length, 
                    const float x, const float y, 
                    const bool centeredX, const bool centeredY, const float size) const;

    void renderText3D(const char *text, const uint length, const float x, const float y, const float z,
                      const float height=1.0f, const bool centeredX=true) const;
    
    void renderOutline(CCVector3 start, CCVector3 end, const float multiple) const;
    
    void view() const;

protected:
    virtual void bindTexturePage() const = 0;

    typedef struct
    {
        CCPoint start, end;
        CCSize size;
    } Letter;
    const Letter* getLetter(const char character) const;
    
protected:
    const char *name;
    
    enum { num_letters = 256 };
    Letter letters[num_letters];
};


#endif // __TEXTUREFONTPAGE_H__
