#version 330
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved



out vec4 color;
in vec3 normal;
in vec3 fragCoord;
in vec3 tangent;
in vec3 bitangent;
uniform vec3 cameraPos;


uniform sampler2D background;
uniform sampler2D grassSampler;
uniform sampler2D rockSampler;
uniform sampler2D alphaSampler;
uniform sampler2D roadSampler;
uniform sampler2D roadSpecularSampler;
uniform sampler2D normalSampler;
uniform vec2 screenSize;

const vec3 dirToLight = normalize(vec3(1, 3, 1));
float specular = 0;
vec3 nnormal = normal;
//Calculates the visible surface color based on the Blinn-Phong illumination model
vec4 calculateLighting(vec4 materialColor, float specularIntensity, vec3 normalizedNormal, vec3 directionToViewer)
{
    vec4 color = materialColor;
    vec3 h = normalize(dirToLight + directionToViewer);
    color.xyz *= 0.9 * max(dot(normalizedNormal, dirToLight), 0) + 0.1;
    color.xyz += specularIntensity * pow(max(dot(h, normalizedNormal), 0), 50);
    return color;
}

vec4 getRoadTexture(sampler2D sampler0){
    return texture(sampler0, mod(fragCoord.xz / 255, 1));
}


vec4 getBackgroundColor()
{
    return texture(background, gl_FragCoord.xy / screenSize);
}

float getSlope(){
    return 1.0f - normal.y;
}

vec4 getTexture(sampler2D sampler0){
    return texture(sampler0, mod(fragCoord.xz / 255 * 10, 1));
}


void readNormal(){
    vec3 normalTexture = getTexture(normalSampler).xyz;
    vec3 tanNormal = 2 * normalTexture - 1;
    mat3 MT = inverse(mat3(tangent,bitangent,normal));
    nnormal =  normalize(tanNormal * MT);
}

vec4 getTexture()
{

    float slope = getSlope();
    vec4 grassTexture = getTexture(grassSampler);
    vec4 rockTexture = getTexture(rockSampler);

    vec4 alphaTexture = getRoadTexture(alphaSampler);
    vec4 color;

    float revSlope = 1 - slope;
    float blend =  min(1, (1- revSlope * revSlope) * 2f);
    color = mix(grassTexture, rockTexture, blend);

    if (alphaTexture.x >= 0.25){
        specular = getTexture(roadSpecularSampler).x;//grey
        readNormal();
    }
    vec4 roadTexture = getTexture(roadSampler);

    return mix(color, roadTexture, alphaTexture.x);

}



void main()
{
    //surface geometry
    vec3 dirToViewer = cameraPos - fragCoord;

    //material properties
    color = getTexture();

    //Calculate light
    color = calculateLighting(color, specular, nnormal, dirToViewer);


}