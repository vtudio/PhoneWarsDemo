/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 Ð 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceFileManager.h
 * Description : Android specific file manager.
 *
 * Created     : 15/05/11
 * Author(s)   : Chris Bowers, Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCDEVICEFILEMANAGER_H__
#define __CCDEVICEFILEMANAGER_H__

#include "CCFileManager.h"

#include "../libzip/zip.h"

class CCBaseFile
{
public:
	virtual const bool open(const char *file) = 0;
	virtual void close() = 0;
	virtual const uint read(void *dest, const uint size) = 0;
	virtual void seek(const uint size) = 0;
	virtual void setPosition(const uint pos) = 0;
	const bool endOfFile()
	{
		return m_Position >= m_Size;
	}

	virtual const uint size()
	{
		return m_Size;
	}

	virtual const uint position()
	{
		return m_Position;
	}

protected:
	uint m_Size;
	uint m_Position;
};

class CCDeviceFileManager : public CCFileManager
{
public:
	CCDeviceFileManager(CCResourceType resourceType);

	const bool open(const char *file);
	void close();
	const uint read(void *dest, const uint size);
	void seek(const uint size);
	void setPosition(const uint pos);
	const bool endOfFile();
	const uint size();
	const uint position();

public:
    static CCText apkPath;
    static CCText dataPath;

private:
    CCBaseFile *m_File;
};

class CCZipFile : public CCBaseFile
{
public:
	CCZipFile();

	const bool open(const char *file);
	void close();
	const uint read(void *dest, const uint size);
	void seek(const uint size);
	void setPosition(const uint pos);

protected:
	zip_file *m_File;

private:
	zip *m_apkArchive;
};

class CCBinaryFile : public CCBaseFile
{
public:
	CCBinaryFile();

	const bool open(const char *file);
	void close();
	const uint read(void *dest, const uint size);
	void seek(const uint size);
	void setPosition(const uint pos);

protected:
	FILE *m_File;
};


#endif // __CCDEVICEFILEMANAGER_H__
