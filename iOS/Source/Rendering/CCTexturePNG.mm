/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTexturePNG.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCTexturePNG.h"
#import "CCTextureManager.h"
#import "CCDeviceFileManager.h"


CCTexturePNG::CCTexturePNG()
{
}


CCTexturePNG::~CCTexturePNG()
{
}


#define uint8 unsigned char
#define uint32	unsigned int
static uint32 NextPowerOfTwo(uint32 x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	return x + 1;
}


#define USE_CG

bool CCTexturePNG::load(const char *path, const CCResourceType resourceType, const bool generateMipMap) 
{	
    CCText fullFilePath;
    if( resourceType == Resource_Cached )
    {
        fullFilePath = CCDeviceFileManager::GetDocsFolder();
    }
    else
    {
        fullFilePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
    }
    
    CCText filename( path );
    filename.stripDirectory();
    
    fullFilePath += "/";
    fullFilePath += filename.buffer;

    
#ifdef USE_CG
    
    // Use CG routines to load texture...
	const bool isPNG = strstr( path, ".png" ) != NULL;
    
	CGDataProviderRef cgDataProviderRef = CGDataProviderCreateWithFilename( fullFilePath.buffer );
	if( cgDataProviderRef != NULL )
	{
		// Load image
		CGImageRef image = NULL;
		if( isPNG )
        {
            image = CGImageCreateWithPNGDataProvider( cgDataProviderRef, NULL, false, kCGRenderingIntentDefault );
        }
		else
        {
            image = CGImageCreateWithJPEGDataProvider( cgDataProviderRef, NULL, false, kCGRenderingIntentDefault );
        }
		ASSERT( image != NULL );
		CGDataProviderRelease( cgDataProviderRef );
        
		// Get information about image
		CGBitmapInfo info = CGImageGetBitmapInfo(image);
		CGColorSpaceModel colormodel = CGColorSpaceGetModel( CGImageGetColorSpace( image ) );
		size_t bpp = CGImageGetBitsPerPixel( image );
		if( bpp < 8 || bpp > 32 || ( colormodel != kCGColorSpaceModelMonochrome && colormodel != kCGColorSpaceModelRGB ) )
		{
			// This loader does not support all possible CCCmage types, such as paletted images
			CGImageRelease( image );
            ASSERT( false );
            return false;
		}
		
		// Decide formats using bpp
		GLenum format, internal;
		switch( bpp )
		{
                // Easy formats
			case 24 : { internal = format = GL_RGB; break; }
			case 16 : { internal = format = GL_LUMINANCE_ALPHA; break; }
			case 8  : { internal = format = GL_LUMINANCE; break; }
				
                // Possibly trickier format
			case 32 :
			{
				internal = format = GL_RGBA;
				switch( info & kCGBitmapAlphaInfoMask )
				{
					case kCGImageAlphaFirst:
					case kCGImageAlphaNoneSkipFirst:
					case kCGImageAlphaPremultipliedFirst:
					{
						// Alpha first requires swap
						format = GL_BGRA;
						break;
					}
				}
				break;
			}
				
                // Error
			default:
			{
                // Couldn't handle image bpp
				CGImageRelease( image );
                ASSERT( false );
				return false;
			}
		}
        
		// Get pixel data
		CFDataRef data = CGDataProviderCopyData( CGImageGetDataProvider( image ) );
        ASSERT( data != NULL );
        
		GLubyte *pixels = (GLubyte*)CFDataGetBytePtr( data );
		ASSERT( pixels != NULL );
        
		allocatedWidth = imageWidth = CGImageGetWidth( image );
		allocatedHeight = imageHeight = CGImageGetHeight( image );
        uint32 components = ( bpp >> 3 );
		
		// Shuffle image data if format is one we don't support
		uint rowBytes = CGImageGetBytesPerRow( image );
		static bool BGRASupport = extensionSupported( "GL_IMG_texture_format_BGRA8888" );
		if( format == GL_BGRA )
		{
			uint imgWide = rowBytes / components;
			uint num = imgWide * imageHeight;
			uint32_t *p = (uint32_t*)pixels;
			
			if ((info & kCGBitmapByteOrderMask) != kCGBitmapByteOrder32Host)
			{
				// Convert from ARGB to BGRA
				for( uint i=0; i<num; ++i )
                {
                    p[i] = (p[i] << 24) | ((p[i] & 0xFF00) << 8) | ((p[i] >> 8) & 0xFF00) | (p[i] >> 24);
                }
			}
			
			// All current iPhoneOS devices support BGRA via an extension.
			if( !BGRASupport )
			{
				format = GL_RGBA;
				
				// Convert from BGRA to RGBA
				for( uint i=0; i<num; ++i )
                {
#if __LITTLE_ENDIAN__
					p[i] = ((p[i] >> 16) & 0xFF) | (p[i] & 0xFF00FF00) | ((p[i] & 0xFF) << 16);
#else
                    p[i] = ((p[i] & 0xFF00) << 16) | (p[i] & 0xFF00FF) | ((p[i] >> 16) & 0xFF00);
#endif
                }
			}
		}
        
		// Remove alpha from non-png formats
		uint8* srcPixel = (uint8*)pixels;
		uint8* destPixel = (uint8*)pixels;
		if( ( bpp==32 ) && ( !isPNG ) )
		{
			uint32 count = imageWidth * imageHeight;
			if( format == GL_RGBA )
			{
				while( count )
				{
					destPixel[0] = srcPixel[0];
					destPixel[1] = srcPixel[1];
					destPixel[2] = srcPixel[2];
					destPixel += 3;
					srcPixel += 4;
					--count;
				}
			}
			else
			{
				// First 4 pixels will corrupt each other, so use special swap
				uint32 fCount = 4;
				while ((fCount)&&(count))
				{
					uint32 p = *(uint32*)srcPixel;
					uint8* p8 = (uint8*)&p;
					destPixel[2] = p8[0];
					destPixel[1] = p8[1];
					destPixel[0] = p8[2];
					destPixel += 3;
					srcPixel += 4;
					--fCount;
					--count;
				}
				
				// Handle the rest of the pixels
				while (count)
				{
					destPixel[2] = srcPixel[0];
					destPixel[1] = srcPixel[1];
					destPixel[0] = srcPixel[2];
					destPixel += 3;
					srcPixel += 4;
					--count;
				}
			}
            
			internal = format = GL_RGB;
			rowBytes -= ( rowBytes >> 2 );
			bpp = 24;
            components = ( bpp >> 3 );
		}
        
        // Fix power of two issues
        GLubyte *tempBuffer = NULL;
		static bool limitedNPOT = extensionSupported( "GL_APPLE_texture_2D_limited_npot" );
        if( !limitedNPOT )
		{
            allocatedWidth = NextPowerOfTwo( imageWidth );
			allocatedHeight = NextPowerOfTwo( imageHeight );
			if( imageWidth != allocatedWidth || imageHeight != allocatedHeight )
			{
                GLuint dstBytes = allocatedWidth * components;
                tempBuffer = (GLubyte*)malloc( dstBytes * allocatedHeight );
				
				for( uint32 y=0; y<imageHeight; ++y )
                {
					memcpy( &tempBuffer[y*dstBytes], &pixels[y*rowBytes], rowBytes );
                }
				
				pixels = tempBuffer;
				rowBytes = dstBytes;
			}
		}
        
        // How much space is the texture?
        allocatedBytes = allocatedWidth * allocatedHeight * components;
        
		// Stuff pixels into an OpenGL texture
		glGenTextures( 1, &glName );
        gEngine->textureManager->bindTexture( glName );
		
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        if( generateMipMap )
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        }
        else
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        }
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		
        glTexImage2D( GL_TEXTURE_2D, 0, internal, allocatedWidth, allocatedHeight, 0, format, GL_UNSIGNED_BYTE, pixels );
        
        if( generateMipMap )
        {
            glGenerateMipmap( GL_TEXTURE_2D );
        }
        
        if( tempBuffer != NULL )
        {
            free( tempBuffer );
        }
		
		// Clean up
		CFRelease( data );
		CGImageRelease( image );
        
        gEngine->textureManager->bindTexture( 0 );
        
        return true;
	}

#else // USE_UIIMAGE
    
    // Re-uses UIImage pipe, however doesn't support anything that isn't 32 bits RGBA
    NSString *nsPath = [[NSString alloc] initWithUTF8String:fullFilePath.buffer];
    NSData *texData = [[NSData alloc] initWithContentsOfFile:nsPath];
    [nsPath release];
    
    UIImage *uiImage = [[UIImage alloc] initWithData:texData];
    [texData release];
    
    ASSERT( uiImage != NULL );
    if( uiImage != NULL )
    {
        CGImageRef image = uiImage.CGImage;
        
        // Get Image size
        allocatedWidth = imageWidth = CGImageGetWidth( image );
        allocatedHeight = imageHeight = CGImageGetHeight( image );
        
        // Allocate memory for image
        void *imageData = (GLubyte*)calloc( imageHeight * imageWidth * 4, sizeof( GLubyte ) );
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        CGBitmapInfo imgInfo = CGImageGetBitmapInfo( image );
        size_t bpp = CGImageGetBitsPerPixel( image );
        //ASSERT( imgInfo != 0 );
        //ASSERT( bpp == 32 );
        
        uint32 components = ( bpp >> 3 );
        
        GLenum format;
        if( bpp == 32 )
        {
            format = GL_RGBA;
        }
        else if( bpp == 24 )
        {
            format = GL_RGB;
        }
        else
        {
            ASSERT( false );
        }
        
        CGContextRef imgContext = CGBitmapContextCreate( imageData, imageWidth, imageHeight, 8, 4 * imageWidth, colorSpace, 
                                                         kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
        CGColorSpaceRelease( colorSpace );
        
        CGContextClearRect( imgContext, CGRectMake( 0, 0, imageWidth, imageHeight ) );
        CGContextDrawImage( imgContext, CGRectMake( 0, 0, imageWidth, imageHeight ), image );
        
        // Fix power of two issues
		static bool limitedNPOT = extensionSupported( "GL_APPLE_texture_2D_limited_npot" );
		if( !limitedNPOT )
		{
            allocatedWidth = NextPowerOfTwo( imageWidth );
			allocatedHeight = NextPowerOfTwo( imageHeight );
			if( imageWidth != allocatedWidth || imageHeight != allocatedHeight )
			{
                uint rowBytes = CGImageGetBytesPerRow( image );
                GLubyte *pixels = (GLubyte*)imageData;
                
				GLuint dstBytes = allocatedWidth * components;
				GLubyte *temp = (GLubyte*)malloc( dstBytes * allocatedHeight );
				
				for( uint32 y=0; y<imageHeight; ++y )
                {
					memcpy( &temp[y*dstBytes], &pixels[y*rowBytes], rowBytes );
                }
				
                free( imageData );
				imageData = temp;
				rowBytes = dstBytes;
			}
		}

        glGenTextures( 1, &glName );
        gEngine->textureManager->bindTexture( glName );
        
        // Set a few parameters to handle drawing the image at lower and higher sizes than original
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        //glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
        
        // Generate texture in opengl
        glTexImage2D( GL_TEXTURE_2D, 0, format, allocatedWidth, allocatedHeight, 0, format, GL_UNSIGNED_BYTE, imageData );
        
        // Release context
        CGContextRelease( imgContext );
        
        // Free Stuff
        free( imageData );
        [uiImage release];
        
        gEngine->textureManager->bindTexture( 0 );
        
        return true;
    }
    
#endif
	
	return false;
}


bool CCTexturePNG::createGLTexture()
{
	return true;	
}


bool CCTexturePNG::extensionSupported(const char *extension)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;
	
    // Extension names should not have spaces
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0') return 0;
	
    // It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
    extensions = glGetString(GL_EXTENSIONS);
    start = extensions;
    for (;;)
	{
        where = (GLubyte*)strstr( (const char*)start, extension );
        if (!where) break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return true;
        start = terminator;
    }
    return false;
}