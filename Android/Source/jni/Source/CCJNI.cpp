/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCJNI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCJNI.h"

#include "CCAppManager.h"


void CCJNI::Assert(const char *file, const int line, const char *message)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "Assert", "(Ljava/lang/String;ILjava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring jFile = jEnv->NewStringUTF( file );
	jstring jMessage = jEnv->NewStringUTF( message );
	jEnv->CallStaticVoidMethod( jniClass, mid, jFile, line, jMessage );
	jEnv->DeleteLocalRef( jFile );
	jEnv->DeleteLocalRef( jMessage );
}


void CCJNI::AdvertsToggle(const bool toggle)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "AdvertsToggle", "(Z)V" );
	ASSERT( mid != 0 );

	// Call the function
	jEnv->CallStaticVoidMethod( jniClass, mid, toggle );
}


void CCJNI::WebBrowserOpen(const char *url)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "WebBrowserOpen", "(Ljava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring javaURL = jEnv->NewStringUTF( url );
	jEnv->CallStaticVoidMethod( jniClass, mid, javaURL );
	//env->DeleteLocalRef( javaURL );
}


void CCJNI::VideoViewStart(const char *file)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "startVideoView", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "VideoViewStart", "(Ljava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring javaURL = jEnv->NewStringUTF( file );
	jEnv->CallStaticVoidMethod( jniClass, mid, javaURL );
	//jEnv->DeleteLocalRef( javaURL );
}


void CCJNI::VideoViewStop()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "stopVideoView", which takes 0 parameters, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "VideoViewStop", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	jEnv->CallStaticVoidMethod( jniClass, mid );
}


static CCLambdaCallback *billingCallback = NULL;
void CCJNI::BillingRequestPurchase(const char *productID, CCLambdaCallback *callback)
{
	if( billingCallback != NULL )
	{
		delete billingCallback;
	}
	billingCallback = callback;

	CCText androidProductID = productID;
	androidProductID.toLowercase();

	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "startVideoView", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "BillingRequestPurchase", "(Ljava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring javaURL = jEnv->NewStringUTF( androidProductID.buffer );
	jEnv->CallStaticVoidMethod( jniClass, mid, javaURL );
	//jEnv->DeleteLocalRef( javaURL );
}


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_BillingItemPurchased(JNIEnv *jEnv, jobject jObj)
{
	if( billingCallback != NULL )
	{
		gEngine->nativeToEngineThread( billingCallback );
		billingCallback = NULL;
	}
}


void CCJNI::GCMRegister()
{
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "stopVideoView", which takes 0 parameters, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "GcmRegister", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	jEnv->CallStaticVoidMethod( jniClass, mid );
}


void CCJNI::GCMUnregister()
{
	JNIEnv *jEnv = gView->jniEnv;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "stopVideoView", which takes 0 parameters, and returns void
	static jmethodID mid = jEnv->GetStaticMethodID( jniClass, "GcmUnregister", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	jEnv->CallStaticVoidMethod( jniClass, mid );

}
