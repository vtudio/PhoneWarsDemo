/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCBaseTypes.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


// CCBaseType
CCBaseType::~CCBaseType()
{
}



// CCData
CCData::CCData()
{
    zero();
}


void CCData::zero()
{
    length = 0;
    buffer = NULL;
    bufferSize = 0;
}


CCData::~CCData()
{
    if( buffer != NULL )
    {
        free( buffer );
    }
}


void CCData::setSize(const uint inLength)
{
    if( inLength > 0 )
    {
        length = inLength+1;
        if( length > bufferSize )
        {
            if( buffer != NULL )
            {
                free( buffer );
            }
            
            // 32 byte aligned
            bufferSize = ( round( length / 32 ) + 1 ) * 32;
            buffer = (char*)malloc( bufferSize );
        }
        buffer[inLength] = 0;
    }
    else
    {
        if( buffer != NULL )
        {
            free( buffer );
        }
        zero();
    }
}


void CCData::set(const char *data, const uint inLength)
{
    setSize( inLength );
    if( inLength > 0 )
    {
        memcpy( buffer, data, inLength );
    }
}



// CCText
CCText::CCText(const int inLength)
{
    ensureLength( inLength );
}


CCText::CCText(const char *text)
{
    zero();
    set( text );
}


CCText::CCText(const CCText &other)
{
    zero();
    set( other.buffer );
}


bool CCText::operator==(const char *other) const
{
    if( length > 0 )
    {
        return ( strcmp( buffer, other ) == 0 );
    }
    return false;
}


bool CCText::operator!=(const char *other) const
{
    if( length > 0 )
    {
        return ( strcmp( buffer, other ) != 0 );
    }
    return false;
}


CCText& CCText::operator=(const char *text)
{
    set( text );
    return *this;
}


CCText& CCText::operator=(const CCText &other)
{
    if( this != &other )
    {
        if( other.buffer != NULL )
        {
            set( other.buffer );
        }
        else if( buffer != NULL )
        {
            free( buffer );
            buffer = NULL;
            bufferSize = 0;
        }
    }
    return *this;
}


CCText& CCText::operator+=(const char *other)
{
    if( other != NULL )
    {
        if( length == 0 )
        {
            set( other );
        }
        else
        {
            const uint otherLength = strlen( other );
            const uint totalLength = length + otherLength;
            ensureLength( totalLength, true );

            strcat( buffer, other );
            length = totalLength;
        }
    }
    return *this;
}


CCText& CCText::operator+=(const char value)
{
    char other[32];
    sprintf( other, "%c", value );
    *this += other;
    return *this;
}


CCText& CCText::operator+=(const int value)
{
    char other[32];
    sprintf( other, "%i", value );
    *this += other;
    return *this;
}


CCText& CCText::operator+=(const long long value)
{
    char other[64];
    sprintf( other, "%lld", value );
    *this += other;
    return *this;
}


CCText& CCText::operator+=(const float value)
{
    char other[32];
    sprintf( other, "%f", value );
    *this += other;
    return *this;
}


void CCText::set(const char *text)
{
    if( text != NULL )
    {
        length = strlen( text );
        uint minLength = length < 31 ? 31 : length;
        ensureLength( minLength );

        strcpy( buffer, text );
    }
}


void CCText::clear()
{
    set( "" );
}


void CCText::ensureLength(const uint minLength, const bool keepBuffer)
{
    const uint newSize = sizeof( char ) * ( minLength + 1 );
    if( newSize > bufferSize )
    {
        char *oldBuffer = buffer;
        bufferSize = newSize;
        buffer = (char*)malloc( bufferSize );

        if( oldBuffer != NULL )
        {
            if( keepBuffer )
            {
                strcpy( buffer, oldBuffer );
            }

            free( oldBuffer );
        }
    }
}


void CCText::trimLength(const uint maxLength)
{
    if( length > maxLength )
    {
        length = maxLength;
        buffer[length] = '\0';
    }
}


CCList<char>* CCText::split(const char *token, const bool first)
{
    const uint tokenLength = strlen( token );
    CCList<char> *splitList = new CCList<char>();
    if( buffer )
    {
        char *previousFind = NULL;
        char *currentFind = buffer;
        splitList->add( currentFind );
        do
        {
            previousFind = currentFind;
            currentFind = strstr( currentFind, token );
            if( currentFind != NULL )
            {
                *currentFind = 0;
                currentFind += tokenLength;
                if( currentFind != buffer )
                {
                    splitList->add( currentFind );
                    
                    if( first )
                    {
                        break;
                    }
                }
            }
        } while( currentFind != NULL && currentFind != previousFind );
    }
    return splitList;
}


bool CCText::Equals(const CCText &text, const CCText &token)
{
    return Equals( text.buffer, token.buffer );
}


bool CCText::Equals(const CCText &text, const char *token)
{
    return Equals( text.buffer, token );
}


bool CCText::Equals(const char *buffer, const char *token)
{
    if( buffer == NULL && token == NULL )
    {
        return true;
    }
    if( buffer != NULL && token != NULL )
    {
        return strcmp( buffer, token ) == 0;
    }
    return false;
}


bool CCText::Contains(const char *buffer, const char *token)
{
    if( buffer != NULL && token != NULL )
    {
        return strstr( buffer, token ) != NULL;
    }
    return false;
}


bool CCText::StartsWith(const char *buffer, const char *token)
{
    if( buffer != NULL )
    {
        const char *find = strstr( buffer, token );
        if( find == buffer )
        {
            return true;
        }
    }
    return false;
}


void CCText::stripExtension()
{
    length -= 4;
    buffer[length] = 0;
}


void CCText::stripDirectory()
{
    CCList<char> *directorySplit = split( "/" );
    if( directorySplit->length > 1 )
    {
        set( directorySplit->list[directorySplit->length-1] );
    }
    DELETE_POINTER( directorySplit );
}


void CCText::strip(const char *token)
{
    CCList<char> *tokenSplit = split( token );
    if( tokenSplit->length > 1 )
    {
        set( tokenSplit->list[0] );
        for( int i=1; i<tokenSplit->length; ++i )
        {
            *this += tokenSplit->list[i];
        }
    }
    DELETE_POINTER( tokenSplit );
}


void CCText::toLowercase()
{
	for( uint i=0; i<length; ++i )
	{
		char lowercase = tolower( buffer[i] );
		buffer[i] = lowercase;
	}
}


void CCText::SetLastWord(const char *inBuffer, CCText &outText)
{
    CCText nameData = inBuffer;
    CCList<char> *nameDataSplit = nameData.split( " " );
    ASSERT( nameDataSplit->length > 0 );
    outText = nameDataSplit->list[nameDataSplit->length-1];
    DELETE_POINTER( nameDataSplit );
}


void CCText::replaceChar(const char search, const char replace)
{
    for( uint i=0; i<length; ++i )
    {
        if( buffer[i] == search )
        {
            buffer[i] = replace;
        }
    }
}


void CCText::replaceChars(const char *token, const char *replace)
{
    CCList<char> *tokenSplit = split( token );
    CCText newText;
    for( int i=0; i<tokenSplit->length; ++i )
    {
        const char *rawData = tokenSplit->list[i];
        newText += rawData;
        
        if( i < tokenSplit->length-1 )
        {
            newText += replace;
        }
    }
    DELETE_POINTER( tokenSplit );
    
    set( newText.buffer );
}


void CCText::splitBetween(CCText source, const char *from, const char *to)
{
    CCText &result = source;
    result.splitAfter( result, from );
    result.splitBefore( result, to );
    set( result.buffer );
}


void CCText::splitBefore(CCText source, const char *before)
{
    CCList<char> *list1 = source.split( before, true );
    set( list1->list[0] );
    DELETE_POINTER( list1 );
}


void CCText::splitAfter(CCText source, const char *after)
{
    CCList<char> *list1 = source.split( after, true );
    if( list1->length > 1 )
    {
        set( list1->list[1] );
        for( int i=2; i<list1->length; ++i )
        {
            *this += after;
            *this += list1->list[i];
        }
    }
    DELETE_POINTER( list1 );
}


void CCText::removeBetween(const char *from, const char *to)
{
    CCText start( buffer );
    start.splitAfter( start, to );
    CCText end( buffer );
    end.splitBefore( end, from );

    *this = end.buffer;
    *this += from;
    *this += to;
    *this += start.buffer;
}


void CCText::removeBetweenIncluding(const char *from, const char *to)
{
    CCText start( buffer );
    CCList<char> *list1 = split( from );
    if( list1->length > 1 )
    {
        CCText end( list1->list[1] );
        start.set( list1->list[0] );
        end.splitAfter( end, to );

        set( start.buffer );
        *this += end.buffer;
    }
    DELETE_POINTER( list1 );
}
