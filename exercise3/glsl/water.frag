#version 330

in vec3 fragCoord;
in vec4 clipSpace;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
uniform vec3 cameraPos;
uniform sampler2D background;
//uniform sampler2D reflectionTexture;
//uniform sampler2D refractionTexture;
uniform sampler2D waterTexture;
uniform sampler2D waterNormal;
uniform sampler2D waterSpecular;
uniform vec2 screenSize;

out vec4 color;
const vec3 dirToLight = normalize(vec3(1, 3, 1));

float specular = 0;
vec3 nnormal;
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

vec4 calculateFog(vec4 color, int fogMinDist, int fogMaxDist){
    /* fog */
    float dist = length(fragCoord - cameraPos);
    float fogFactor = (fogMaxDist - dist) / (fogMaxDist - fogMinDist);
    fogFactor = 1 - clamp(fogFactor, 0.0, 1.0);
    vec4 backgroundColor = getBackgroundColor();
    color = mix(color, backgroundColor, fogFactor);
    return color;
}

vec4 getTexture(sampler2D sampler0){
    return texture(sampler0, mod(fragCoord.xz / 255 * 10, 1));
}

void readNormal(){
    vec3 normalTexture = getTexture(waterNormal).xyz;
    vec3 tanNormal = 2 * normalTexture - 1;
    mat3 MT = inverse(mat3(tangent, bitangent, normal));
    nnormal =  normalize(tanNormal * MT);
}

void readSpecular(){
    specular = getTexture(waterSpecular).r;//grey
}

void main(){

   // vec2 ndc = (gl_FragCoord.xz / gl_FragCoord.w) / 2.0 + 0.5;
   // vec2 refractTexCoords = vec2(ndc.x, ndc.y);
   // vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
//
  //  vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
  //  vec4 refractColor = texture(refractionTexture, refractTexCoords);
    //color = mix(reflectColor,refractColor,0.5);
    readNormal();
    readSpecular();
    color = getTexture(waterTexture);
   // color = mix(color, refractColor, 0);
   // color = mix(color, reflectColor, 0);
    vec3 dirToViewer = cameraPos - fragCoord;

    color = calculateLighting(color, specular,nnormal, dirToViewer);
    color = calculateFog(color, 500, 1000);

}