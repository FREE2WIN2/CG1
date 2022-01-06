#version 130
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

in vec4 fragment_color;
in vec2 position;

uniform float zoom;
uniform vec2 seed;

out vec4 color;



void main(void)
{
    /** 2.2.2
    color = fragment_color;
    */
    /**** Begin of tasks ***
    - 2.2.5
    Implement the pseudo-code for calculating the julia fractal at a point.
    To get the point you can declare a new "in" variable which contains the
    position and just use the X- and Y- value.  ***/

    vec2 z = position * zoom;
    int imax = 200;
    int i = 1;
    for (i;i<=imax;i++){
        float x = (z.x * z.x - z.y * z.y) + seed.x;
        float y = (z.y * z.x + z.x * z.y) + seed.y;
        if ((x * x ) + (y * y) > 4.0f){
            break;
        }
        z.x = x;
        z.y = y;
    }

    float alpha = 0.0;
    if(i<imax){
        alpha = float(i) / float(imax);
    }

    color = vec4(alpha) * 10.0f * fragment_color;


    /*** End of tasks ***/

}