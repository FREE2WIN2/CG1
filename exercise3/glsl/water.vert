#version 330

in vec4 position;
in vec2 offset;

uniform mat4 mvp;

out vec4 fragCoord;
out float alpha;

float getTerrainHeight(vec2 p);

void main(){
    vec4 offsetPosition = position + vec4(offset.x, 0, offset.y, 0);
    float y = getTerrainHeight(offsetPosition.xz);
    //out
    alpha = max(0, min((offsetPosition.y - y) / 4, 1)); // 0 -> 1 at difference of 3 we have alpha = 1
    fragCoord = offsetPosition;
    gl_Position = mvp * (fragCoord);
}

float rand(vec2 c)
{
    return 2 * fract(sin(dot(c.xy, vec2(12.9898, 78.233))) * 43758.5453) - 1;
}

float perlinNoise(vec2 p)
{
    vec2 ij = floor(p);
    vec2 xy = p - ij;
    //xy = 3.*xy*xy-2.*xy*xy*xy;
    xy = .5*(1.-cos(3.1415926 * xy));
    float a = rand((ij+vec2(0., 0.)));
    float b = rand((ij+vec2(1., 0.)));
    float c = rand((ij+vec2(0., 1.)));
    float d = rand((ij+vec2(1., 1.)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

//based on https://www.seedofandromeda.com/blogs/58-procedural-heightmap-terrain-generation
float getTerrainHeight(vec2 p)
{
    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 1.0;
    float frequency = 0.02;
    for (int i = 0; i < 11; i++)
    {
        total +=  ((1.0 - abs(perlinNoise(p * frequency))) * 2.0 - 1.0) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= 0.45;
    }
    return 15 * total / maxAmplitude;
}