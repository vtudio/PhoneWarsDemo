/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTextureFontPageFile.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCTextureFontPageFile.h"
#include "CCFileManager.h"


CCTextureFontPageFile::CCTextureFontPageFile(const char *inName)
{
    name = inName;
}


CCTextureFontPageFile::~CCTextureFontPageFile()
{
}


bool CCTextureFontPageFile::load(const char *path, const CCResourceType resourceType, const bool generateMipMap)
{
    CCText textureFile( path );
    textureFile += ".png";
    texturePageIndex = gEngine->textureManager->assignTextureIndex( textureFile.buffer, resourceType, generateMipMap, true, true );
    
    CCText dataFile( path );
    dataFile += ".data";
    
	// Load the descriptor file
    char *fileData = NULL;
    CCFileManager::GetPackagedFile( dataFile.buffer, &fileData );
    CCText textData( fileData );
    FREE_POINTER( fileData );
    
    CCList<char> *lettersSplit = textData.split( "\n" );
    CCText rawLetterData;
    for( int i=0; i<lettersSplit->length; ++i )
    {
        const char *raw = lettersSplit->list[i];
        rawLetterData.set( raw );
        
        CCList<char> *letterDataSplit = rawLetterData.split( "," );
        ASSERT( letterDataSplit->length == 4 );
        
        Letter &letter = letters[i];
        const char *x1 = letterDataSplit->list[0];
        const char *y1 = letterDataSplit->list[1];
        const char *x2 = letterDataSplit->list[2];
        const char *y2 = letterDataSplit->list[3];
                             
        letter.start.x = (float)atof( x1 );
        letter.start.y = (float)atof( y1 );
        letter.end.x = (float)atof( x2 );
        letter.end.y = (float)atof( y2 );

        // 16.0f because there's 16 tiles per font page
        letter.size.width = ( letter.end.x - letter.start.x ) * 16.0f;
        letter.size.height = ( letter.end.y - letter.start.y ) * 16.0f;
        
        DELETE_POINTER( letterDataSplit );
    }
    DELETE_POINTER( lettersSplit );
	
	return true;
}


void CCTextureFontPageFile::bindTexturePage() const
{
	gEngine->textureManager->setTextureIndex( texturePageIndex );
}
