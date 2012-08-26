/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : alphacolour.fx
 * Description : Used to draw fonts.
 *
 * Created     : 08/09/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

precision mediump float;

// Globals
uniform highp mat4 u_modelViewProjectionMatrix;
uniform vec4 u_modelColour;

// PS Input
varying vec2 ps_texCoord;


#ifdef VERTEX_SHADER

// VS Input
attribute highp vec4 vs_position;
attribute vec2 vs_texCoord;

void main()
{
    gl_Position = u_modelViewProjectionMatrix * vs_position;
    ps_texCoord = vs_texCoord;
}

#endif


#ifdef PIXEL_SHADER

uniform sampler2D s_diffuseTexture;

void main()
{
    gl_FragColor.rgb = u_modelColour.rgb;
    gl_FragColor.a = u_modelColour.a * texture2D( s_diffuseTexture, ps_texCoord ).a;
}

#endif
