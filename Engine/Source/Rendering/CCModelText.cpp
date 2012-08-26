/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCModelText.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCTextureManager.h"
#include "CCTextureFontPage.h"


CCModelText::CCModelText(CCObjectCollideable *inParent)
{
    primitive = NULL;
    parent = NULL;
    setParent( inParent );
    
    shader = "alphacolour";
    
    colourInterpolator.setDuration( 0.5f );
}


void CCModelText::destruct()
{
    super::destruct();
}


bool CCModelText::update(const float delta)
{
    return colourInterpolator.update( delta );
}


void CCModelText::setParent(CCObjectCollideable *inParent)
{
    ASSERT( parent == NULL );
    parent = inParent;
}


void CCModelText::setText(const char *text, const float height, const char *font)
{
    if( primitive == NULL )
    {
        translate( 0.0f, 0.0f, CC_SMALLFLOAT );
        setColour( CCColour( 0.0f ), false );
        primitive = new CCPrimitiveText( text );
        addPrimitive( primitive );
        
        setFont( gEngine->textureManager->fontPages.first()->getName() );
    }
    else
    {
        primitive->setText( text );
    }
    
    if( height != -1.0f )
    {
        setHeight( height );
    }
    
    if( font != NULL )
    {
        setFont( font );
    }
}


void CCModelText::setHeight(const float height)
{
    primitive->setHeight( height );
}


void CCModelText::setCentered(const bool centered)
{
    if( centered )
    {
        setPositionX( 0.0f );
    }
    else if( parent != NULL )
    {
        setPositionX( -parent->collisionBounds.x );
    }
    primitive->setCentered( centered );
}


void CCModelText::setColour(const CCColour &inColour, const bool interpolate)
{
    if( interpolate )
    {
        colourInterpolator.setup( colour, inColour );
    }
    else 
    {
        super::setColour( inColour );
        colourInterpolator.setup( colour, *colour );
    }
}


void CCModelText::setColourAlpha(const float inAlpha, const bool interpolate, CCLambdaCallback *inCallback)
{
    if( interpolate )
    {
        colourInterpolator.setTargetAlpha( inAlpha, inCallback );
    }
    else 
    {
        super::setColourAlpha( inAlpha );
        colourInterpolator.setup( colour, *colour );
    }
}


void CCModelText::setFont(const char *font)
{
    primitive->setFont( font );
}



// CCPrimitiveText
CCPrimitiveText::CCPrimitiveText(const char *inText, const float inHeight)
{	
    setText( inText );
    setHeight( inHeight );
    setCentered( true );
    endMarker = false;
}


void CCPrimitiveText::destruct()
{	
	super::destruct();
}


// CCPrimitiveBase
void CCPrimitiveText::renderVertices(const bool textured)
{	
    if( text.buffer != NULL )
    {
        fontPage->renderText3D( text.buffer, text.length, 
                                0.0f, 0.0f, 0.0f,
                                height,
                                centered );
        
        if( endMarker )
        {
            gEngine->textureManager->setTextureIndex( 0 );
            const float x = ( fontPage->getWidth( text.buffer, text.length, height ) + fontPage->getCharacterWidth( ' ', height ) ) * 0.5f;
            const float y = height * 0.45f;
            const CCVector3 start = CCVector3( x, -y, 0.0f );
            const CCVector3 end = CCVector3( x, y, 0.0f );
            CCRenderLine( start, end );
        }
    }
}


void CCPrimitiveText::setText(const char *inText)
{
    text = inText;
}


void CCPrimitiveText::setFont(const char *font)
{
    for( int i=0; i<gEngine->textureManager->fontPages.length; ++i )
    {
        CCTextureFontPage *page = gEngine->textureManager->fontPages.list[i];
        const char *name = page->getName();
        if( CCText::Equals( font, name ) )
        {
            fontPage = page;
            return;
        }
    }
    ASSERT( false );
}


float CCPrimitiveText::getWidth()
{
    return fontPage->getWidth( text.buffer, text.length, height );
}


float CCPrimitiveText::getHeight()
{
    return fontPage->getHeight( text.buffer, text.length, height );
}
