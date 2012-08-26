/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCRenderTools.h
 * Description : Helper rendering functions.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCRENDERTOOLS_H__
#define __CCRENDERTOOLS_H__


// Matrix functions
//-----------------
// Obsolete please use CCMatrix.h implementation
extern void CCMatrixMulVec(const float matrix[16], const float in[4], float out[4]);
extern void CCMatrixMulMat(const float a[16], const float b[16], float r[16]);
extern bool CCMatrixInvert(const float m[16], float invOut[16]);

// Render functions
//-----------------
extern void CCRenderSquare(const CCPoint &start, const CCPoint &end, const bool outline=false);
extern void CCRenderSquare(const CCVector3 &start, const CCVector3 &end);
extern void CCRenderSquareYAxisAligned(const CCVector3 &start, const CCVector3 &end);
extern void CCRenderSquarePoint(const CCPoint &position, const float &size);
extern void CCRenderRectanglePoint(const CCPoint &position, const float &sizeX, const float &sizeY, const bool outline=false);

extern void CCRenderLine(const CCVector3 &start, const CCVector3 &end);
extern void CCRenderCube(const bool outline);
extern void CCRenderCubeMinMax(const CCVector3 min, const CCVector3 max, const bool outline);

// Shader functions
//-----------------
extern void CCSetColour(const CCColour &colour);
extern const CCColour& CCGetColour();

extern void CCSetTexCoords(const float *inUVs);
extern void CCDefaultTexCoords();
extern void CCInverseTexCoords();

extern void CCRefreshRenderAttributes();


#endif // __CCRENDERTOOLS_H__
