//----------------------------------------------------------------------------------------
/**
 * \file       mandelbrot.frag
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Mandelbrot fragment shader.
 *
 *  Computes Mandelbrot set coloring per pixel and outputs the final fractal image.
 *
*/
//----------------------------------------------------------------------------------------
#version 140
// Mandelbrot fragment shader.
// Renders animated Mandelbrot set with palette-based color cycling.

// -------------------------------- Macros ------------------------------------

#define HALF 0.5
#define ONE 1.0
#define TAU 6.28318

// ------------------------------- Uniforms -----------------------------------

uniform float elapsedTime;      // elapsed application time
uniform vec3  asteroidLocation; // reserved uniform kept for program compatibility

uniform bool  mandelbrotAnimStarted;   // animation enabled by interaction
uniform bool  mandelbrotAnimPaused;    // animation pause flag
uniform float mandelbrotAnimStartTime; // absolute time when animation started
uniform float mandelbrotAnimPauseTime; // absolute time captured on pause

uniform int   mandelbrotMaxIterations; // bailout loop limit
uniform float mandelbrotZoomSpeed;     // reserved zoom speed parameter
uniform float mandelbrotColorSpeed;    // palette animation speed
uniform vec2  mandelbrotZoomtarget;    // zoom center in fractal coordinates

// ------------------------------- Attributes ---------------------------------

in vec2 UV; // normalized quad UV coordinates

out vec4 fragmentColor; // final RGBA output

// ------------------------------- Functions ----------------------------------


// Palette equation based on cosine interpolation:
// https://iquilezles.org/articles/palettes/
vec3 palette(float t) {
    vec3 a = vec3(HALF, HALF, HALF);
    vec3 b = vec3(HALF, HALF, HALF);
    vec3 c = vec3(ONE, ONE, ONE);
    vec3 d = vec3(0.18, 0.333, 0.498);
    
    return a + b*cos(TAU*(c*t + d));
}

// ############################################################################
//                                  Main
// ############################################################################

void main() {
    // animation time starts only after object interaction
    float animatedTime = 0.0;

    // animation only if picked
    if (mandelbrotAnimStarted) {
        float mandelbrotTime = elapsedTime;
        if (mandelbrotAnimPaused)
            mandelbrotTime = mandelbrotAnimPauseTime;

        animatedTime = max(mandelbrotTime - mandelbrotAnimStartTime, 0.0);
    }

    // map board UVs to the complex-plane viewing window
    vec2 uv = UV * 3.0;
    uv.x -= 0.85;
    uv.y -= 1.8;

    // smooth zoom towards configured target
    vec2 target = mandelbrotZoomtarget;
    float zoom = 1.0;
    zoom = exp(-0.2 * animatedTime);
    uv = target + uv * zoom;
    
    float real          = uv.x;
    float imaginary     = uv.y;
    float OG_real       = real;
    float OG_imaginary  = imaginary;
    
    float dist_sq = 0.0;     //distance squared from origin
    int iterations = 0;      //number of iterations until divergence
    
    // Mandelbrot iteration loop z = z^2 + c
    for(int i = 0; i < mandelbrotMaxIterations && dist_sq < 4.0; ++i) {
        float tempReal = real;
        
        real = (tempReal*tempReal) - (imaginary*imaginary) + OG_real;
        imaginary = 2.0*tempReal*imaginary + OG_imaginary;
        dist_sq = (real*real) + (imaginary*imaginary);
        iterations++;
    }

    vec3 color = vec3(1.0);
    
    // divergent points get palette color, convergent points remain black
    if(dist_sq > 4.0) {
        float t = float(iterations) / 20.0;
        color = palette(t + animatedTime * mandelbrotColorSpeed);
    } else color = vec3(0.0);

    fragmentColor = vec4(color, 1.0);
}
