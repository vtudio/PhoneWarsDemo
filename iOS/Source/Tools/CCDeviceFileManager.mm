/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceFileManager.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCDeviceFileManager.h"


CCDeviceFileManager::CCDeviceFileManager(CCResourceType resourceType)
{
	m_File		= NULL;
	m_Size		= 0;
	m_Position	= 0;
}


const bool CCDeviceFileManager::open(const char *filePath)
{
	m_File = fopen( filePath, "rb" );
	if( m_File == NULL )
	{
		return false;
	}
    
	fpos_t pos = 0;
	fseek( m_File, 0, SEEK_END );
	fgetpos( m_File, &pos );
	fseek( m_File, 0, SEEK_SET );
	m_Size		= (uint)pos;
	m_Position	= 0;
	
	return true;
}


void CCDeviceFileManager::close()
{
	if( m_File != NULL )
	{
		fclose( m_File );
		m_File = NULL;
	}
	
	m_File = NULL;
	m_Size = 0;
	m_Position = 0;
}


const uint CCDeviceFileManager::read(void *dest, const uint size)
{
	ASSERT_MESSAGE( m_File != NULL, "File::Read(...) : ERROR! File not open" );
	
	// Regular file handle
	const uint sz = fread( dest, size, 1, m_File );
    ASSERT( sz == 1 );
	if( sz > 0 )
	{
		m_Position += size;
		return size;
	}
	DEBUGLOG( "File::Read(...) : WARNING! Error reading. Code %i\n", sz );
	ASSERT_MESSAGE( false, "File::Read(...) : ERROR! Error reading" );
    
	return 0;
}



void CCDeviceFileManager::seek(const uint size)
{
	ASSERT_MESSAGE( m_File != NULL, "File::Position(...) : ERROR! File not open" );
    
	fseek( m_File, size, SEEK_CUR );
    m_Position += size;
}


void CCDeviceFileManager::setPosition(const uint pos)
{
	ASSERT_MESSAGE( m_File != NULL, "File::Position(...) : ERROR! File not open" );
	ASSERT_MESSAGE( pos < m_Size, "File::Position(...) : ERROR! Invalid file position" );
    
	fseek( m_File, pos, SEEK_SET );
	m_Position = pos;
}


const bool CCDeviceFileManager::endOfFile()
{
	return m_Position >= m_Size; 
}


const uint CCDeviceFileManager::size()
{
	return m_Size; 
}


const uint CCDeviceFileManager::position()
{ 
	return m_Position; 
}


const char* CCDeviceFileManager::GetDocsFolder()
{
    static CCText docsDirectory;
    if( docsDirectory.length == 0 )
    {
        docsDirectory = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];
        docsDirectory += "/";
    }
    return docsDirectory.buffer;
}
