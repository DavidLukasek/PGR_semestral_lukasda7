#version 140

#define HALF 0.5
#define ONE 1.0
#define TAU 6.28318
#define MAX_ITER 1000

uniform float elapsedTime;

in vec2 UV;

out vec4 fragmentColor;

//palette equation:     https://iquilezles.org/articles/palettes/
//palette parameters:   https://dev.thi.ng/gradients/
vec3 palette(float t) {
    vec3 a = vec3(HALF, HALF, HALF);
    vec3 b = vec3(HALF, HALF, HALF);
    vec3 c = vec3(ONE, ONE, ONE);
    vec3 d = vec3(0.18, 0.333, 0.498);
    
    return a + b*cos(TAU*(c*t + d));
}

void main() {
    const float WIN_HEIGHT = 500.0;
    
    //center coordinates scaled up to <-2;2>
    vec2 uv = UV * 3.0;
    uv.x -= 2.0;
    uv.y -= 1.5;
    
    //slow zoom
    //vec2 target = vec2(-1.2506715, 0.0201205);
    //float zoom = 1.0;
    // zoom = exp(-0.2 * elapsedTime);
    //uv = target + uv * zoom;
    
    float real          = uv.x;
    float imaginary     = uv.y;
    float OG_real       = real;
    float OG_imaginary  = imaginary;
    
    float dist_sq = 0.0;     //distance squared from origin
    int iterations = 0;      //number of iterations until divergence
    
    //fractal loop
    for(int i = 0; i < MAX_ITER && dist_sq < 4.0; ++i) {
        float tempReal = real;
        
        real = (tempReal*tempReal) - (imaginary*imaginary) + OG_real;
        imaginary = 2.0*tempReal*imaginary + OG_imaginary;
        dist_sq = (real*real) + (imaginary*imaginary);
        iterations++;
    }

    vec3 color = vec3(1.0);
    
    if(dist_sq > 4.0) {
        float t = float(iterations) / 20.0;
        color = palette(t + elapsedTime*0.2);
    } else color = vec3(0.0);

    fragmentColor = vec4(color, 1.0);
}
