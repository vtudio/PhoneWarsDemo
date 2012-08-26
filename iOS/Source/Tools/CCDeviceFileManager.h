/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceFileManager.h
 * Description : iOS specific file manager.
 *
 * Created     : 09/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCFileManager.h"

class CCDeviceFileManager : public CCFileManager
{
public:
	CCDeviceFileManager(CCResourceType resourceType);
	
	const bool open(const char *filePath);
	void close();
	const uint read(void *dest, const uint size);
    void seek(const uint size);
	void setPosition(const uint pos);
	const bool endOfFile();
	const uint size();
	const uint position();
    
    static const char* GetDocsFolder();
	
protected:
	FILE *m_File;
	uint m_Size;
	uint m_Position;
};
