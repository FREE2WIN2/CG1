#version 330
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved



out vec4 color;
in vec3 normal;
in vec3 fragCoord;
uniform vec3 cameraPos;


uniform sampler2D background;
uniform sampler2D grassSampler;
uniform sampler2D rockSampler;
uniform sampler2D alphaSampler;

uniform vec2 screenSize;

const vec3 dirToLight = normalize(vec3(1, 3, 1));
//Calculates the visible surface color based on the Blinn-Phong illumination model
vec4 calculateLighting(vec4 materialColor, float specularIntensity, vec3 normalizedNormal, vec3 directionToViewer)
{
    vec4 color = materialColor;
    vec3 h = normalize(dirToLight + directionToViewer);
    color.xyz *= 0.9 * max(dot(normalizedNormal, dirToLight), 0) + 0.1;
    color.xyz += specularIntensity * pow(max(dot(h, normalizedNormal), 0), 50);
    return color;
}

vec4 getBackgroundColor()
{
    return texture(background, gl_FragCoord.xy / screenSize);
}

float getSlope(){
    return 1.0f - normal.y;
}

vec4 getTexture(sampler2D sampler0){
    return texture(sampler0, fragCoord.xz / 255 * 10);
}

vec4 getTexture()
{
    float slope = getSlope();
    vec4 grassTexture = getTexture(grassSampler);
    vec4 rockTexture = getTexture(rockSampler);

    if (slope < 0.2){
        return grassTexture;
    }
    if (slope < 0.7){
        float blend = (slope - 0.2) * 2.0f;
        return mix(rockTexture,grassTexture, blend);
    }
    return rockTexture;
}

void main()
{
    //surface geometry
    vec3 dirToViewer = vec3(0, 1, 0);

    //material properties
    color = getTexture();
    float specular = 0;


    //Calculate light
    color = calculateLighting(color, specular, normal, dirToViewer);


}