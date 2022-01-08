#version 330

in vec4 fragCoord;
in float alpha;

uniform vec3 cameraPos;
uniform sampler2D waterSampler;

out vec4 color;

const vec3 dirToLight = normalize(vec3(1, 3, 1));

vec4 calculateLighting(vec4 materialColor, float specularIntensity, vec3 normalizedNormal, vec3 directionToViewer)
{
    vec4 color = materialColor;
    vec3 h = normalize(dirToLight + directionToViewer);
    color.xyz *= 0.9 * max(dot(normalizedNormal, dirToLight), 0) + 0.1;
    color.xyz += specularIntensity * pow(max(dot(h, normalizedNormal), 0), 50);
    return color;
}

vec4 getTexture(sampler2D sampler0){
    return texture(sampler0, mod(fragCoord.xz / 255 * 10, 1));
}


void main(){
    color = getTexture(waterSampler);

    color = calculateLighting(color, 0, vec3(0, 1, 0), cameraPos);
    color.a = alpha;
}