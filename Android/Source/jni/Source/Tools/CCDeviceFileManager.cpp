/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceFileManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCDeviceFileManager.h"

#include <errno.h>
#include <jni.h>


CCText CCDeviceFileManager::apkPath;
CCText CCDeviceFileManager::dataPath;


// Sets up the path to the resources in the APK file
extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_FileManagerSetPaths(JNIEnv *jEnv, jobject jObj, jstring jApkPath, jstring jDataPath)
{
	// Do the string conversion before setting the path
	jboolean isCopy;
	const char *temp = jEnv->GetStringUTFChars( jApkPath, &isCopy );
	CCDeviceFileManager::apkPath = temp;
	jEnv->ReleaseStringUTFChars( jApkPath, temp );

	temp = jEnv->GetStringUTFChars( jDataPath, &isCopy );
	CCDeviceFileManager::dataPath = temp;
	CCDeviceFileManager::dataPath += "/";
	jEnv->ReleaseStringUTFChars( jDataPath, temp );
}


CCDeviceFileManager::CCDeviceFileManager(CCResourceType resourceType)
{
	if( resourceType == Resource_Packaged )
	{
		m_File = new CCZipFile();
	}
	else
	{
		m_File = new CCBinaryFile();
	}
}


const bool CCDeviceFileManager::open(const char *file)
{
	return m_File->open( file );
}


void CCDeviceFileManager::close()
{
	return m_File->close();
}


const uint CCDeviceFileManager::read(void *dest, const uint size)
{	
	return m_File->read( dest, size );
}

void CCDeviceFileManager::seek(const uint size)
{
	m_File->seek( size );
}


void CCDeviceFileManager::setPosition(const uint pos)
{
	m_File->setPosition( pos );
}


const bool CCDeviceFileManager::endOfFile()
{
    return m_File->endOfFile();
}


const uint CCDeviceFileManager::size()
{
    return m_File->size();
}


const uint CCDeviceFileManager::position()
{ 
    return m_File->position();
}


// CCZipFile
CCZipFile::CCZipFile()
{
	m_apkArchive  	= NULL;
	m_File			= NULL;
	m_Size			= 0;
	m_Position		= 0;
}


const bool CCZipFile::open(const char *file)
{
	// Grab the APK (zip) file to access resource
	m_apkArchive = zip_open( CCDeviceFileManager::apkPath.buffer, 0, NULL );
	if( m_apkArchive == NULL )
	{
		DEBUGLOG( "Error loading APK" );
		return false;
	}

    // It's not an image, so add res/raw/ to the path to look in the correct directory
	CCText fullFilePath = "res/raw/";
    fullFilePath += file;

	// Open the supplied file as read-only
	m_File = zip_fopen( m_apkArchive, fullFilePath.buffer, 0 );
	if( m_File != NULL )
	{
		// Get the file size
		struct zip_stat stat;
		zip_stat( m_apkArchive, fullFilePath.buffer, 0, &stat );
		m_Size = stat.size;
		return true;
	}

	DEBUGLOG( "Error loading file %s: %s", fullFilePath.buffer, strerror( errno ) );
    return false;
}


void CCZipFile::close()
{
	if( m_File != NULL )
	{
		zip_fclose( m_File );
	}
	m_File = NULL;
    m_Size = 0;

	if( m_apkArchive != NULL )
	{
		zip_close( m_apkArchive );
	}
	m_apkArchive = NULL;
}


const uint CCZipFile::read(void *dest, const uint size)
{
	ASSERT( m_File != NULL && m_apkArchive != NULL );

	// Regular file handle
	const uint sz = zip_fread( m_File, dest, size );
	ASSERT( sz == size );
	if( sz > 0 )
	{
		m_Position += size;
		return size;
	}
	ASSERT_MESSAGE( false, "File::Read(...) : ERROR! Error reading" );

    return 0;
}

void CCZipFile::seek(const uint size)
{
	// Cannot seek through a zip file using libzip so fake a read
	char empty[size];
	read( empty, size );
}


void CCZipFile::setPosition(const uint pos)
{
	// Cannot seek through a zip file using libzip
	ASSERT( false );
//    m_File.seek( pos );
//    m_Position = pos;
}


CCBinaryFile::CCBinaryFile()
{
	m_File		= NULL;
	m_Size		= 0;
	m_Position	= 0;
}


const bool CCBinaryFile::open(const char *filePath)
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


// CCBinaryFile
void CCBinaryFile::close()
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


const uint CCBinaryFile::read(void *dest, const uint size)
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



void CCBinaryFile::seek(const uint size)
{
	ASSERT_MESSAGE( m_File != NULL, "File::Position(...) : ERROR! File not open" );

	fseek( m_File, size, SEEK_CUR );
    m_Position += size;
}


void CCBinaryFile::setPosition(const uint pos)
{
	ASSERT_MESSAGE( m_File != NULL, "File::Position(...) : ERROR! File not open" );
	ASSERT_MESSAGE( pos < m_Size, "File::Position(...) : ERROR! Invalid file position" );

	fseek( m_File, pos, SEEK_SET );
	m_Position = pos;
}
