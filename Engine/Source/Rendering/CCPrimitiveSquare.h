/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCPrimitiveSquare.h
 * Description : Square drawable component.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCPRIMITIVESQUARE_H__
#define __CCPRIMITIVESQUARE_H__


struct CCPrimitiveSquareUVs
{
	static void Setup(CCPrimitiveSquareUVs **uvs, const float x1, const float y1, const float x2, const float y2)
	{
		if( *uvs == NULL )
		{
			*uvs = new CCPrimitiveSquareUVs( x1, y1, x2, y2 );
		}
		else
		{
			(*uvs)->set( x1, y1, x2, y2 );
		}
	}

	CCPrimitiveSquareUVs(const float x1, const float y1, const float x2, const float y2)
	{
		set( x1, y1, x2, y2 );
	}
					 
	void set(const float x1, const float y1, const float x2, const float y2)
	{
		uvs[0] = x1;
		uvs[1] = y2;
		uvs[2] = x2;
		uvs[3] = y2;
		uvs[4] = x1;
		uvs[5] = y1;
		uvs[6] = x2;
		uvs[7] = y1;
	}
	
	void scroll(const float x, const float y)
	{
		uvs[0] += x;
		uvs[1] += y;
		uvs[2] += x;
		uvs[3] += y;
		uvs[4] += x;
		uvs[5] += y;
		uvs[6] += x;
		uvs[7] += y;
		
		if( uvs[0] > 1.0f )
		{
			for( uint i=0; i<8; ++i )
			{
				uvs[i] -= 1.0f;
			}
		}
	}
    
    void flipY()
    {
        const float x1 = uvs[0];
        const float x2 = uvs[2];
        uvs[0] = x2;
		uvs[2] = x1;
		uvs[4] = x2;
		uvs[6] = x1;
    }
	
	float uvs[8];
};

class CCPrimitiveSquare : public CCPrimitiveBase
{
public:
	typedef CCPrimitiveBase super;

	CCPrimitiveSquare();

    // CCBaseType
	virtual void destruct();
	
	// PrimitiveBase
protected:
    // Adjust the model's UVs to match the loaded texture, 
    // as non-square textures load into a square texture which means the mapping requires adjustment
    void adjustTextureUVs();
    
public:
	virtual void renderVertices(const bool textured);
	virtual void renderOutline();
	
    // Face Y axis (upwards)
    void setupYFacing(const float width, const float height, const float depth, const bool useNormals=true, const float direction=1.0f);
    void setupYFacing(const CCPoint start, const CCPoint end, const float depth, const bool useNormals=true, const float direction=1.0f);
	
    // Face X axis (sideways)
    void setupXFacing(const float width, const float height, const float depth);
	
    // Face Z axis (forwards)
    void setupZFacing(const float width, const float height, const float z=0.0f);
    
	void setupPoints(const float tL, const float tR,
                     const float bL, const float bR,
                     const float bY, const float tY,
                     const float bZ, const float tZ);

public:
	CCPrimitiveSquareUVs *customUVs;
};


#endif // __CCPRIMITIVESQUARE_H__
