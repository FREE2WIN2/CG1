#version 330

layout (location = 0) in vec4 position;
in vec2 offset;
uniform mat4 mvp;
uniform vec4 plane;

out vec3 fragCoord;
float getTerrainHeight(vec2 p);

void main(){
    vec4 offsetPosition = position + vec4(offset.x, 0, offset.y, 0);
    gl_ClipDistance[0] = dot(offsetPosition,plane);
    fragCoord = offsetPosition.xyz;
    gl_Position = mvp * (offsetPosition);
}