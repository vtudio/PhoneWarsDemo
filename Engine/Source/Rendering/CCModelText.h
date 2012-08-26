/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCModelText.h
 * Description : Represents a 3d text primitive.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCMODELTEXT_H__
#define __CCMODELTEXT_H__


class CCPrimitiveText : public CCPrimitiveBase
{    
    friend class CCModelText;
    
public:
    typedef CCPrimitiveBase super;

    CCPrimitiveText(const char *inText, const float inHeight=10.0f);

    // CCBaseType
    virtual void destruct();

    // PrimitiveBase
    virtual void renderVertices(const bool textured);

	void setText(const char *inText);
    
    void setFont(const char *font);
    
    void setHeight(const float inHeight) { height = inHeight; }
    void setCentered(const bool toggle) { centered = toggle; }
    void setEndMarker(const bool toggle) { endMarker = toggle; }

    float getWidth();
    float getHeight();
    const CCText& getText() { return text; }
    
	struct CCText text;
    
protected:
    CCTextureFontPage *fontPage;
    float height;
    bool centered;
    bool endMarker;
};

// Here's a model that contains the text primitive to make things easier
class CCModelText : public CCModelBase
{
public:
    typedef CCModelBase super;
    
    CCModelText(CCObjectCollideable *inParent=NULL);
    virtual void destruct();
    
    bool update(const float delta);

    void setParent(CCObjectCollideable *inParent);
    
    const CCText& getText() { return primitive->text; }
    float getWidth() { return primitive->getWidth(); }
    float getHeight() { return primitive->getHeight(); }
    
    void setText(const char *text, const float height=-1.0f, const char *font=NULL);
    void setHeight(const float height);
    void setCentered(const bool centered);
    
	void setColour(const CCColour &inColour, const bool interpolate);
	void setColourAlpha(const float inAlpha, const bool interpolate, CCLambdaCallback *inCallback=NULL);
    CCInterpolatorLinearColour& getColourInterpolator() { return colourInterpolator; }
private:
    // Don't use these directly, use the above versions
    void setColour(const CCColour &inColour) { super::setColour( inColour ); }
    void setColourAlpha(const float inAlpha) { super::setColourAlpha( inAlpha ); }
    
public:
    void setFont(const char *font);
    
    void setEndMarker(const bool toggle) { primitive->setEndMarker( toggle ); }
    bool getEndMarker() { return primitive->endMarker; }
    
protected:
    CCPrimitiveText *primitive;
    CCObjectCollideable *parent;
    
    CCInterpolatorLinearColour colourInterpolator;
};


#endif // __CCMODELTEXT_H__
