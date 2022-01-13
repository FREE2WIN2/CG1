#version 330

in vec4 position;
in vec2 offset;

uniform int time;
uniform mat4 mvp;
uniform vec4 plane;
uniform sampler2D waterHeightMap;
out vec3 fragCoord;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

float getHeight(vec2 uv, float y){

    vec2 offset1 = vec2(0.8, 0.4) * time * 0.008;
    vec2 offset2 = vec2(0.6, 1.1) * time * 0.008;
    float hight1 = texture2D(waterHeightMap, uv + offset1).r * 1.5;
    float hight2 = texture2D(waterHeightMap, uv + offset2).r * 1.5;
    return y + hight1 + hight2;
}

vec2 toUV(float x, float y){
    return mod(vec2(x, y)/255*10, 1);
}

void calcNormal(vec3 vertice){
    float sampleOffset = 1.0f;
    float x = vertice.x;
    float y = vertice.y;
    float z = vertice.z;
    float hL = getHeight(toUV(x - sampleOffset, z),y);
    float hR = getHeight(toUV(x + sampleOffset, z),y);
    float hD = getHeight(toUV(x, z - sampleOffset),y);
    float hU = getHeight(toUV(x, z + sampleOffset),y);
    normal = normalize(vec3(hL - hR, sampleOffset * 2, hD - hU));

    tangent = cross(vec3(1, 0, 0), normal);
    bitangent = cross(vec3(0, 0, 1), normal);
}



void main(){
    vec4 offsetPosition = position + vec4(offset.x, 0, offset.y, 0);
    float y = getHeight(toUV(offsetPosition.x, offsetPosition.z),position.y);
    offsetPosition.y = y;

    calcNormal(offsetPosition.xyz);
    gl_ClipDistance[0] = dot(offsetPosition, plane);
    fragCoord = offsetPosition.xyz;
    gl_Position = mvp * (offsetPosition);
}