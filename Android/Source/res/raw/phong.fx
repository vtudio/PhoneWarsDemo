/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : phong.fx
 * Description : Basic phong lighting.
 *
 * Created     : 08/09/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

precision mediump float;

// -------
// Globals
// -------
uniform highp mat4 u_modelViewProjectionMatrix;
uniform highp mat4 u_modelViewMatrix;
uniform vec4 u_modelColour;

//#ifdef LIGHTING_ENABLED
uniform mat4 u_modelNormalMatrix;
uniform vec3 u_lightPosition;
uniform vec4 u_lightDiffuse;
//#endif


// ------------------
// VS Output/PS Input
// ------------------
varying vec2 ps_texCoord;

//#ifdef LIGHTING_ENABLED
varying highp vec3 ps_worldPosition;
varying vec3 ps_worldNormal;
//#endif


// -----------------
#ifdef VERTEX_SHADER
// -----------------
// VS Input
attribute highp vec4 vs_position;
attribute vec2 vs_texCoord;

//#ifdef LIGHTING_ENABLED
attribute vec3 vs_normal;
//#endif

void main()
{
    gl_Position = u_modelViewProjectionMatrix * vs_position;
    ps_texCoord = vs_texCoord;

//#ifdef LIGHTING_ENABLED
    ps_worldPosition = vec3( u_modelViewMatrix * vs_position );

#ifdef QT
    ps_worldNormal = vs_normal;
    ps_worldNormal = vec3( 1, 1, 1 );
#else
    // Works for uniform scaled models
    // normal.w must be 0.0 to kill off translation
    vec3 transformedNormal = vec3( u_modelViewMatrix * vec4( vs_normal, 0.0 ) );
    ps_worldNormal = normalize( transformedNormal );
#endif
    // Normal matrix is required for non-uniform scaled models
//    vec3 transformedNormal = ( u_modelNormalMatrix * vec4( vs_normal, 1.0 ) ).xyz;
//    ps_worldNormal = normalize( transformedNormal );
//#endif
}

#endif



// ----------------
#ifdef PIXEL_SHADER
// ----------------

uniform sampler2D s_diffuseTexture;

void main()
{
    vec4 textureColour = texture2D( s_diffuseTexture, ps_texCoord ).rgba;

//#ifdef LIGHTING_ENABLED

    vec3 u_lightPosition = vec3( -100.0, 100.0, 100.0 );
    vec4 u_lightDiffuse = vec4( 1.0, 1.0, 1.0, 1.0 );

    vec3 L = normalize( u_lightPosition - ps_worldPosition );
    vec4 Idiff = u_lightDiffuse * max( dot( ps_worldNormal, L ), 0.0 );
    Idiff = clamp( Idiff, 0.0, 1.0 );
    Idiff.a = 1.0;

    gl_FragColor = Idiff * u_modelColour * textureColour;

//#else
//
//    gl_FragColor = u_modelColour * textureColour;
//
//#endif
}

#endif
