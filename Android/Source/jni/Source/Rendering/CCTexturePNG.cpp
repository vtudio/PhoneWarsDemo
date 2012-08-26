/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 Ð 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTexturePNG.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCTexturePNG.h"
#include "CCTextureManager.h"
#include "CCDeviceFileManager.h"

#include "CCGLView.h"


CCTexturePNG::CCTexturePNG()
{
}


CCTexturePNG::~CCTexturePNG()
{
}


static bool JniDoesTextureExist(const char *name, const bool packaged)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureDoesTextureExist", "(Ljava/lang/String;Z)Z" );
	ASSERT( mid != 0 );

	// Call the function
	jstring jFilename = jEnv->NewStringUTF( name );
	const bool result = jEnv->CallStaticBooleanMethod( jniClass, mid, jFilename, packaged );
	jEnv->DeleteLocalRef( jFilename );
	return result;
}


static int JniLoad(const char *name, const bool packaged, const bool generateMipMap)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureLoad", "(Ljava/lang/String;ZZ)I" );
	ASSERT( mid != 0 );

	// Call the function
	jstring jFilename = jEnv->NewStringUTF( name );
	const int result = jEnv->CallStaticIntMethod( jniClass, mid, jFilename, packaged, generateMipMap );
	jEnv->DeleteLocalRef( jFilename );
	return result;
}


static int JniGetImageWidth()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureGetImageWidth", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallStaticIntMethod( jniClass, mid );
}


static int JniGetImageHeight()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureGetImageHeight", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallStaticIntMethod( jniClass, mid );
}


static int JniGetRawWidth()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureGetRawWidth", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallStaticIntMethod( jniClass, mid );
}


static int JniGetRawHeight()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureGetRawHeight", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallStaticIntMethod( jniClass, mid );
}


static int JniGetAllocatedHeight()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "TextureGetAllocatedHeight", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallStaticIntMethod( jniClass, mid );
}


bool CCTexturePNG::load(const char *path, const CCResourceType resourceType, const bool generateMipMap)
{
#if defined PROFILEON
    CCProfiler profile( "CCTexturePNG::load()" );
#endif

	CCText filename = path;
	filename.stripDirectory();
	filename.toLowercase();

	glName = JniLoad( filename.buffer, resourceType == Resource_Packaged, generateMipMap );
	if( glName > 0 )
	{
		allocatedWidth = imageWidth = JniGetImageWidth();
		imageHeight = JniGetImageHeight();
		allocatedHeight = JniGetAllocatedHeight();

		rawWidth = JniGetRawWidth();
		rawHeight = JniGetRawHeight();

		// TODO: pretend it's always 4 bytes on Android, look into getting actual pixel size data
		allocatedBytes = allocatedWidth * allocatedHeight * 4;
		return true;
	}
	return false;
}


bool CCTexturePNG::createGLTexture()
{
	return true;
}


bool CCTexturePNG::DoesTextureExist(const char *path, const CCResourceType resourceType)
{
	CCText filename = path;
	filename.stripDirectory();
	filename.toLowercase();
	return JniDoesTextureExist( filename.buffer, resourceType == Resource_Packaged );
}
