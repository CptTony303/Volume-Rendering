#version 330 core
#extension GL_ARB_gpu_shader_int64 : enable
//#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable

out vec4 FragColor;
  

in vec3 modelPos; //intersection with cube in model position

float optical_thickness = 0.f;
float accumulated_transmittance = 1.f;

uniform sampler3D texture3; //texture of the volume
uniform float isovalue; //treshold for the density
uniform int brightness;
uniform float stepSize;
uniform mat4 model; //model matrix
uniform mat4 view; //view matrix
uniform int methode;
uniform int samplesPerFrame;
uniform float randomizer;


/*BEGIN Implementierung von uint64*/
struct uint64{
    uint msd;
    uint lsd;
};
uint64 newUint64(uint msd, uint lsd){
    uint64 newUint64;
    newUint64.lsd = lsd;
    newUint64.msd = msd;
    return newUint64;
}
uint64 copyUint64(uint64 val){
    return newUint64(val.msd, val.lsd);
}
uint64 addUint64(uint64 a, uint64 b){
    uint64 result;
    result.lsd = a.lsd + b.lsd;
    result.msd = a.msd + b.msd + (result.lsd < a.lsd ? 1u : 0u);
    return result;
}
uint64 mulUint64(uint64 a, uint64 b) {
    uint64 result;
    result.lsd = a.lsd * b.lsd;
    result.msd = a.msd * b.lsd + a.lsd * b.msd + (result.lsd < a.lsd * b.msd ? a.msd : 0u);
    return result;
}
uint64 rightShiftUint64(uint64 value, uint amount) {
    uint64 result;

    result.msd = value.msd >> amount;
    result.lsd = (value.lsd >> amount) | (value.msd << (32u - amount));

    return result;
}
uint64 leftShiftUint64(uint64 value, uint amount) {
    uint64 result;

    result.msd = value.msd << amount | (value.lsd >> (32u - amount));
    result.lsd = (value.lsd << amount);

    return result;
}
uint64 bitwiseOrUint64(uint64 a, uint64 b) {
    uint64 result;
    result.msd = a.msd | b.msd;
    result.lsd = a.lsd | b.lsd;
    return result;
}
uint64 xorUint64(uint64 a, uint64 b) {
    uint64 result;
    result.msd = a.msd ^ b.msd;
    result.lsd = a.lsd ^ b.lsd;
    return result;
}
uint uint64ToUint(uint64 val){
    return val.lsd;
}
/*END Implementierung von uint64*/


/*BEGIN Implementierung RNG*/
#ifndef PCG32_H
#define PCG32_H

struct pcg32_random_t {    // Internals are *Private*.
    uint64 state;             // RNG state.  All values are possible.
    uint64 inc;               // Controls which RNG sequence (stream) is
// selected. Must *always* be odd.
};

uint pcg32_random_r(inout pcg32_random_t rng)
{
    uint64 oldstate = copyUint64(rng.state);
    rng.state = addUint64(mulUint64(oldstate, newUint64(1481765933u,1284865837u)), rng.inc);
    uint xorshifted = uint64ToUint(rightShiftUint64(xorUint64(rightShiftUint64(oldstate, 18u), oldstate), 27u));
    uint rot = uint64ToUint(rightShiftUint64(oldstate, 59u));
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31u));
}

// pcg32_srandom(initstate, initseq)
// pcg32_srandom_r(rng, initstate, initseq):
//     Seed the rng.  Specified in two parts, state initializer and a
//     sequence selection constant (a.k.a. stream id)
void pcg32_srandom_r(inout pcg32_random_t rng, uint64 initstate, uint64 initseq)
{
    rng.state = newUint64(0u, 0u);
    rng.inc = bitwiseOrUint64(leftShiftUint64(initseq, 1u), newUint64(0u,1u));
    pcg32_random_r(rng);
    rng.state = addUint64(rng.state, initstate);
    pcg32_random_r(rng);
}


float pcg32_random_float_r(inout pcg32_random_t rng) {
    return uintBitsToFloat(0x3f800000u | (pcg32_random_r(rng) >> 9u)) - 1.0;
    //return float(pcg32_random_r(rng)) / float(0xFFFFFFFFu);
}


pcg32_random_t pcg32_global = pcg32_random_t(newUint64(2235320806u,1955588763u ), newUint64(3661511115u, 2495175643u));


float pcg32_random_float() {
    return pcg32_random_float_r(pcg32_global);
}

void pcg32_srandom(uint64 seed, uint64 seq)
{
    pcg32_srandom_r(pcg32_global, seed, seq);
}

#endif
/*END Implementierung RNG*/

/*BEGIN simple substitution for rng*/
/// 1 out, 3 in
float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}
///  3 out, 3 in...
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}
vec3 seed = vec3(0.f);
float get_random_float(){
    //get random number with hash
    float random_number = hash13(seed);
    //increment seed
    seed = random_number * 1000 * seed;
    seed = hash33(seed);
    return random_number;
}
void init_seed(vec3 init_seed){
    seed = init_seed;
    //randomize once
    get_random_float();
}
/*END simple substitution for rng*/

vec3 transferFunction(float value){
    value -= 0.5;
    float red = value > 0 ? value * 2 : 0;
    float green = 1.f - 2* abs(value);
    float blue = value < 0 ? value * -2 : 0;;
    return vec3(red, green, blue);
}

vec4 RayMarch_methode_3(float density, vec3 last_Lx){

        vec3 unit_emission = transferFunction(density);;
        float unit_opacity = density;
        float unit_transmittance = 1.0 - unit_opacity;
        float eps = 1e-8;
        float mu_t = -log(clamp(unit_transmittance, eps, 1.0 - eps));
        vec3 Le = (unit_opacity > eps) ? (unit_emission / unit_opacity) : vec3(0.0);
        float T = exp(-mu_t*stepSize*density);
        vec3 Lx = T * mu_t * Le;
        vec3 L = stepSize * 0.5 * (Lx + last_Lx);
        return vec4(L, unit_opacity * stepSize * brightness);
}
vec4 RayMarch_methode_4(float density, vec4 last_Lx){

        vec3 unit_emission = transferFunction(density);;
        float unit_opacity = density;
        float unit_transmittance = 1.0 - unit_opacity;
        float eps = 1e-8;
        float mu_t = -log(clamp(unit_transmittance, eps, 1.0 - eps));
        vec4 Le = (unit_opacity > eps) ? vec4(unit_emission,unit_opacity) : vec4(0.0);
        float T = exp(-mu_t*stepSize*density);
        vec4 Lx = T * mu_t * Le;
        vec4 L = stepSize * 0.5 * (Lx + last_Lx);
        return L;
}

vec4 rayMarch()
{
    //mat3 rotation = mat3(model);
    vec3 worldPos = vec3(model * vec4(modelPos, 1.0));
    mat4 inverseView = inverse(view);
    mat4 inverseModel = inverse(model);
    vec3 cameraPosition = vec3(inverseView[3]);
    //float dist = length(worldPos - cameraPosition);
    vec3 cameraPositionInModel = vec3(inverseModel * vec4(cameraPosition,1.f) ); //eine von beiden Mult wird schon passen---> kamearapos in Modll space  
    vec3 direction = normalize(modelPos - cameraPositionInModel);

//    vec3 scale;
//    scale.x = length(vec3(model[0]));
//    scale.y = length(vec3(model[1]));
//    scale.z = length(vec3(model[2]));


    //float stepSize = 0.01f; // Schrittweite
    float maxDistance = 1.8; // Maximaler Distanz für den Marsch = 1.8 weil wurzel 3 =diagonale vom 3dim Würfel 

    vec3 currentPosition = modelPos;

    vec4 rgba = vec4(0.0f, 0.0f, 0.0f, 0.f);
    vec3 rgb = vec3(0.f,0.f,0.f);

//    vec3 last_Lx = vec3(0.f, 0.f, 0.f);
    vec4 last_Lx = vec4(0.f);

    float last_density = 0.f;

    for (float i = 0.0; i <= maxDistance; i += stepSize)
    {
        vec3 texCoord = currentPosition+vec3(0.5f);
        float density = texture(texture3, texCoord).r;
        density = density > isovalue ? density : 0.f;

//        vec4 L = RayMarch_methode_3(density, last_Lx); rgba += L; last_Lx = vec3(L) * 2 * (1/stepSize);  //methode 3
        vec4 L = RayMarch_methode_4(density, last_Lx);rgba += L;last_Lx = L * 2 * (1/stepSize);  //methode 4

        currentPosition += direction * stepSize;
    }

    return rgba; //methode 3
}

vec4 deltaTracking(vec3 w){
    vec3 x = modelPos;
    float mu = 10.f;
    int counter = 0;
    while(true){
        if(counter >= 500){
            return vec4(0.f,0.f,0.f,0.f);
        }
        counter++;
        float rng1 = get_random_float();
        float rng2 = get_random_float();

        float t = - log (1 - rng1)/mu;
        x += t * w;

        vec3 texCoord = x+vec3(0.5f);
        float density = texture(texture3, texCoord).r*mu;

        if(rng2 < density/mu){
            return vec4(transferFunction(density/mu),1.f);
        }
        if(distance(x,modelPos) > 1.8){ //out of volume (1.8 ist längste diagonale eines Würfels)
            return vec4(0.2f, 0.3f, 0.3f, 1.0f);
        }
    }
}


vec4 monteCarlo(){
    init_seed(randomizer*modelPos+randomizer);
    vec3 worldPos = vec3(model * vec4(modelPos, 1.0));
    mat4 inverseView = inverse(view);
    mat4 inverseModel = inverse(model);
    vec3 cameraPosition = vec3(inverseView[3]);
    //float dist = length(worldPos - cameraPosition);
    vec3 cameraPositionInModel = vec3(inverseModel * vec4(cameraPosition,1.f) ); //eine von beiden Mult wird schon passen---> kamearapos in Modll space  
    vec3 direction = normalize(modelPos - cameraPositionInModel);

    vec4 color = vec4(0.f);
    //float eps = 1e-1;
    int sample_nr = 0;
    //float change_rate = 1.f;
    while(/*change_rate > eps */ sample_nr <samplesPerFrame){
        vec4 add_color = deltaTracking(direction);
        //change_rate = distance(color/sample_nr, (color+add_color)/(sample_nr+1));
        sample_nr++;
        color += add_color;
    }
    return color/sample_nr;
}

void main()
{
    switch(methode){
    case 0:
        FragColor = rayMarch();
        break;
    case 1:
        FragColor = monteCarlo();
        break;
    case 2:
        init_seed(randomizer*modelPos+randomizer);
        FragColor = vec4(get_random_float(), get_random_float(), get_random_float(), 1.f);
        break;
        //Test rng
        float rn1 = get_random_float();
        if(rn1 < 0.f){
            FragColor = vec4(1.f,0.f,0.f,1.f);
        }else if(rn1 > 1.f){
            FragColor = vec4(0.f,0.f,1.f,1.f);
        }else{
            FragColor = vec4(0.f,1.f,0.f,1.f);
        }
        break;
        //pcg32_srandom(pcg32_global.state, pcg32_global.inc);
        //pcg32_srandom(newUint64(uint(modelPos.x),uint(modelPos.y)), newUint64(uint(modelPos.z), uint(modelPos.x)/2u+1u));
//        float eps = 1e-10;
        //if(abs(pcg32_random_float() - pcg32_random_float()) < eps){
//            FragColor = vec4(1.f,0.f,0.f,1.f);
//        }else{
//            FragColor = vec4(0.f,1.f,0.f,rn);
//        }
//        float rn = pcg32_random_float();
//        if(rn > 1.f || rn < 0){
//            FragColor = vec4(1.f,0.f,0.f,1.f);
//        }else if(rn == 1){
//            FragColor = vec4(0.f,0.f,1.f,1.f);
//        }else{
//            FragColor = vec4(0.f,1.f,0.f,rn);
//        }
//        if(rn == 0.999999851){
//            FragColor = vec4(1.f,0.f,0.f,1.f);
//        }else if(rn > 0.99999985){
//            FragColor = vec4(0.f,1.f,0.f,1.f);
//        }else{
//            FragColor = vec4(0.f,0.f,1.f,1.f);
//        }
//        break;
    default:
        FragColor = vec4(modelPos+vec3(0.5f), 1.f);
    }
}
