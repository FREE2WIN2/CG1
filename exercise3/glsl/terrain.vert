#version 330
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

in vec4 position;
in vec2 offset;


uniform mat4 mvp;
out vec3 normal;
out vec3 fragCoord;
out vec3 tangent;
out vec3 bitangent;
//Returns the height of the procedural terrain at a given xz position
float getTerrainHeight(vec2 p);

void calcNormal(vec3 vertice){
/*
	vec3 neighbour1 = vertice + vec3(sampleOffset,0,0);
	vec3 neighbour2 = vertice + vec3(0,0,sampleOffset);
	neighbour1.y = getTerrainHeight(neighbour1.xz);
	neighbour2.y = getTerrainHeight(neighbour2.xz);


	vec2 uv = 255 / vertice.xz ;
	vec2 uvNeighbour1 = 255 / neighbour1.xz ;
	vec2 uvNeighbour2 = 255 / neighbour2.xz;

	vec3 deltaPos1 = neighbour1 - vertice;
	vec3 deltaPos2 = neighbour2 - vertice;
	vec2 deltaUV1 = uvNeighbour1 - uv;
	vec2 deltaUV2 = uvNeighbour2 - uv;

	float x = neighbour1.y*neighbour2.z - neighbour1.z*neighbour2.y; // x
	float y = neighbour1.z * neighbour2.x - neighbour1.x*neighbour2.z;
	float z = neighbour1.x * neighbour2.y - neighbour1.y*neighbour2.x;
	normal = normalize(vec3(x,y,z));

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
	bitangent =  normalize((deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r);

	normal = normalize(cross(tangent,bitangent));
		*/

	float sampleOffset = 0.1f;
	float x = vertice.x;
	float z = vertice.z;
	float hL = getTerrainHeight(vec2(x - sampleOffset, z));
	float hR = getTerrainHeight(vec2(x + sampleOffset, z));
	float hD = getTerrainHeight(vec2(x, z - sampleOffset));
	float hU = getTerrainHeight(vec2(x, z + sampleOffset));
	normal = normalize(vec3(hL - hR, sampleOffset * 2, hD - hU));

}




void main()
{

	vec4 heightPosition = position + vec4(offset.x,0,offset.y,0);
	heightPosition.y = getTerrainHeight(heightPosition.xz);
	gl_Position = mvp * (heightPosition);
	calcNormal(heightPosition.xyz);
	fragCoord = heightPosition.xyz;
}

//source: https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 c)
{
	return 2 * fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453) - 1;
}

float perlinNoise(vec2 p )
{
	vec2 ij = floor(p);
	vec2 xy = p - ij;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(3.1415926 * xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
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