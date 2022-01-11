#version 330

in vec3 fragCoord;
in float alpha;

uniform vec3 cameraPos;
uniform sampler2D waterSampler;
uniform sampler2D background;
uniform vec2 screenSize;
out vec4 color;

const vec3 dirToLight = normalize(vec3(1, 3, 1));


vec4 getBackgroundColor()
{
    return texture(background, gl_FragCoord.xy / screenSize);
}

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

vec4 calculateFog(vec4 color, int fogMinDist, int fogMaxDist){
    /* fog */
    float dist = length(fragCoord - cameraPos);
    float fogFactor = (fogMaxDist - dist) / (fogMaxDist - fogMinDist);
    fogFactor = 1 - clamp(fogFactor, 0.0, 1.0);
    vec4 backgroundColor = getBackgroundColor();
    color = mix(color,backgroundColor,fogFactor);
    return color;
}

void main(){
    color = getTexture(waterSampler);

    color = calculateLighting(color, 0, vec3(0, 1, 0), cameraPos);
    color = calculateFog(color, 500,1000);

    color.a = alpha;
}