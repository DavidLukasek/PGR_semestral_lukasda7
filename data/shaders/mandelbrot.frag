#version 140

// -------------------------------- Macros ------------------------------------

#define HALF 0.5
#define ONE 1.0
#define TAU 6.28318

// ------------------------------- Uniforms -----------------------------------

uniform float elapsedTime;
uniform vec3  asteroidLocation;

uniform bool  mandelbrotAnimStarted;
uniform bool  mandelbrotAnimPaused;
uniform float mandelbrotAnimStartTime;
uniform float mandelbrotAnimPauseTime;

uniform int   mandelbrotMaxIterations;
uniform float mandelbrotZoomSpeed;
uniform float mandelbrotColorSpeed;
uniform vec2  mandelbrotZoomtarget;

// ------------------------------- Attributes ---------------------------------

in vec2 UV;

out vec4 fragmentColor;

// ------------------------------- Functions ----------------------------------


//palette equation:     https://iquilezles.org/articles/palettes/
//palette parameters:   https://dev.thi.ng/gradients/
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
    float animatedTime = 0.0;

    // animation only if picked
    if (mandelbrotAnimStarted) {
        float mandelbrotTime = elapsedTime;
        if (mandelbrotAnimPaused)
            mandelbrotTime = mandelbrotAnimPauseTime;

        animatedTime = max(mandelbrotTime - mandelbrotAnimStartTime, 0.0);
    }

    //center coordinates scaled up to <-2;2>
    vec2 uv = UV * 3.0;
    uv.x -= 0.85;
    uv.y -= 1.8;

    //slow zoom
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
    
    //fractal loop
    for(int i = 0; i < mandelbrotMaxIterations && dist_sq < 4.0; ++i) {
        float tempReal = real;
        
        real = (tempReal*tempReal) - (imaginary*imaginary) + OG_real;
        imaginary = 2.0*tempReal*imaginary + OG_imaginary;
        dist_sq = (real*real) + (imaginary*imaginary);
        iterations++;
    }

    vec3 color = vec3(1.0);
    
    // color divergent fragments, if convergent then black
    if(dist_sq > 4.0) {
        float t = float(iterations) / 20.0;
        color = palette(t + animatedTime * mandelbrotColorSpeed);
    } else color = vec3(0.0);

    fragmentColor = vec4(color, 1.0);
}
