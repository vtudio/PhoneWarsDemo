/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCJNI.h
 * Description : C++ JNI interface class to CCJNI.java
 *
 * Created     : 08/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCJNI_H__
#define __CCJNI_H__


class CCJNI
{
public:
	static void Assert(const char *file, const int line, const char *message);

	static void AdvertsToggle(const bool toggle);
	
	static void WebBrowserOpen(const char *url);

	static void VideoViewStart(const char *file);
	static void VideoViewStop();

	static void BillingRequestPurchase(const char *productID, CCLambdaCallback* callback);

	static void GCMRegister();
	static void GCMUnregister();
};


#endif // __CCJNI_H__
