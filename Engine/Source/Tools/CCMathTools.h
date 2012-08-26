/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCMathTools.h
 * Description : Collection of math tools.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCVectors.h"

#define CC_SMALLFLOAT 0.01f
#define CC_PI (float)M_PI
const float CC_PI2 = CC_PI*CC_PI;
const float CC_HPI = CC_PI * 0.5f;

#define CC_DEGREES_TO_RADIANS(__ANGLE__) ( CC_PI * (__ANGLE__) / 180.0f )
#define CC_RADIANS_TO_DEGREES(__ANGLE__) ( 180.0f * (__ANGLE__) / CC_PI )

#define CC_SWAP(x,y) x^=y^=x^=y

#define CC_SQUARE(x) x*x

extern bool CCEqualFloat(const float a, const float b);
extern float CCSignFloat(const float value);

extern float CCRandomFloat();
extern float CCRandomDualFloat();
extern int CCRandomDualInt();
extern void CCSwapFloat(float &a, float &b);
extern void CCClampFloat(float &value, const float min, const float max);
extern void CCClampInt(int &value, const int min, const int max);
inline bool CCToTarget(float &value, const float target, const float amount)
{
    if( value != target )
	{
        if( value > target )
        {
            value = MAX( target, value - amount );
		}
		else
        {
            value = MIN( target, value + amount );
		}
		
		return true;
	}

    return false;
}

extern bool CCToRotation(float &current, float target, const float amount);
extern float CCLengthSquared(const float a, const float b);
extern uint CCPowerOf2(const uint value);

extern float CCDistance(const float first, const float second);
extern void CCClampDistance(float &current, const float target, const float offset);

extern float CCDistanceBetweenPoints(const CCPoint &first, const CCPoint &second);
extern float CCDistanceBetweenAngles(const float first, const float second);
extern float CCAngleBetweenPoints(const CCPoint &first, const CCPoint &second);
extern float CCAngleBetweenLines(const CCPoint &line1Start, const CCPoint &line1End, const CCPoint &line2Start, const CCPoint &line2End);

extern void CCClampRotation(float &rotation);

extern void CCRotateAboutX(CCVector3 &rotatedPosition, const float rotation, const CCVector3 &from, const CCVector3 &about);
extern void CCRotateAboutY(CCVector3 &rotatedPosition, const float rotation, const CCVector3 &from, const CCVector3 &about);

extern void CCRotateAboutXY(CCVector3 &rotatedPosition, const float rotationX, const float rotationY, const CCVector3 &from, const CCVector3 &about);

extern void CCRotatePoint(CCPoint &position, const float rotation);

extern bool CCOppositePoints(const CCPoint &pointA, const CCPoint &pointB);

extern float CCAngleTowards(const float fromX, const float fromZ, const float toX, const float toZ);
extern float CCAngleTowards(const CCVector3 &from, const CCVector3 &to);

// Clamp the rotation down to movements of an angle (i.e. only allow right angles 90degrees)
extern void CCLockRotation(float *rotation, const float angle);
