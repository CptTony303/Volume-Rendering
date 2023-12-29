#version 330 core
//#extension GL_ARB_gpu_shader_int64 : enable
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
uint64 leftShiftUint64(uint64 value, uint shift) {
    // Übertragsbit prüfen
    bool carry = (value.lsd >> (32u - shift)) > 0u;

    // Linksverschiebung der niederwertigen 32 Bits
    value.lsd <<= shift;

    // Linksverschiebung der höherwertigen 32 Bits
    value.msd <<= shift;

    // Übertrag in die höherwertigen Bits fügen
    value.msd |= (value.lsd >> 32);

    // Übertragsbit löschen
    value.lsd &= 0xFFFFFFFFu;

    // Bei Bedarf Übertrag prüfen und löschen
    if (carry) {
        value.msd += 1u;
    }

    return value;
}
uint64 bitwiseOrUint64(uint64 a, uint64 b) {
    uint64 result;
    result.msd = a.msd | b.msd;
    result.lsd = a.lsd | b.lsd;
    return result;
}
uint64 powerUint64(uint64 base, uint64 exponent){
    uint64 result = newUint64(1u, 0u);

    while (exponent.lsd > 0u || exponent.msd > 0u) {
        if ((exponent.lsd & 1u) != 0u) {
            result = mulUint64(result, base);
        }

        base = mulUint64(base, base);
        exponent.lsd >>= 1u;
        exponent.msd >>= 1u;
    }
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
    uint64 oldstate = rng.state;
    rng.state = addUint64(mulUint64(oldstate, newUint64(1481765933u,1284865837u)), rng.inc);
    uint xorshifted = uint64ToUint(rightShiftUint64(powerUint64(rightShiftUint64(oldstate, 18u), oldstate), 27u));
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
    rng.state += initstate;
    pcg32_random_r(rng);
}


float pcg32_random_float_r(inout pcg32_random_t rng) {
    return uintBitsToFloat(0x3f800000u | (pcg32_random_r(rng) >> 9u)) - 1.0;
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



vec4 RayMarch_methode_1(float density, float last_density, float distance){
            //dist += distance(model * vec4(currentPosition, 1.f), model * vec4(modelPos,1.f));
            //vec3 color = texCoord;

            // density ist meine vereinfachte Transferfunktion (farbe im Rotkanal)

            float opacity =  stepSize * float(brightness) * density;
            vec4 emmision = vec4(density, 1-density, 0.f, opacity);
            //optical_thickness += stepSize * 0.5 * (density + last_density);
            //Absorption
            float eps = 1e-8;
            float mu_t = -log(clamp(density, eps, 1.0 - eps));
            emmision *= mu_t * exp(-mu_t * distance);
            //optische dicke aufaddieren

            return emmision;
}
vec3 RayMarch_methode_2(float density, float last_density){
        float transmittance = 1.f;
        float eps = 1e-8;
        float mu_t = -log(clamp(1 - density, eps, 1.0 - eps));
        float optical_thickness = mu_t * 0.5 * (density + last_density);
        transmittance = (1/stepSize) * exp(-optical_thickness);
        vec3 emmision_color = vec3(density, 1-density, 0.f);
        accumulated_transmittance *= transmittance;
        return emmision_color * transmittance;
}
vec4 RayMarch_methode_3(float density, vec3 last_Lx){

        vec3 unit_emission = vec3(density);;
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

    vec3 last_Lx = vec3(0.f, 0.f, 0.f);
    float last_density = 0.f;

    for (float i = 0.0; i <= maxDistance; i += stepSize)
    {
        vec3 texCoord = currentPosition+vec3(0.5f);
        float density = texture(texture3, texCoord).r;
        density = density > isovalue ? density : 0.f;

        //rgba += RayMarch_methode_1(density, last_density, i); last_density = density; //methode 1
        //rgb += RayMarch_methode_2(density, last_density); last_density = density; // methode 2
        vec4 L = RayMarch_methode_3(density, last_Lx); rgba += L; last_Lx = vec3(L) * 2 * (1/stepSize);  //methode 3

        currentPosition += direction * stepSize;
    }

    return rgba; //methode 1 + 3
    //return vec4(rgb, 1.f);  //methode 2
}

vec4 deltaTracking(vec3 w){
    vec3 x = modelPos;
    float mu = 1.f;
    while(true){

        float rng1 = pcg32_random_float();
        //rng1=abs(rng1);
        //rng1 = rng1 > 1 ? rng1 - floor(rng1): rng1;
        float rng2 = pcg32_random_float();
        //rng2=abs(rng2);
        //rng2 = rng2 > 1 ? rng2 - floor(rng2): rng2;

        float t = - log (1 - rng1)/mu;
        x += t * w;

        vec3 texCoord = x+vec3(0.5f);
        float density = texture(texture3, texCoord).r;

        if(rng2 < density/mu){
            return vec4(vec3(density),1.f);
        }
        if(distance(x,modelPos) > 1.8){ //out of volume (1.8 ist längste diagonale eines Würfels)
            return vec4(0.f);
        }
    }
}


vec4 monteCarlo(){
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
    while(/*change_rate > eps */ sample_nr <100){
        vec4 add_color = deltaTracking(direction);
        //change_rate = distance(color/sample_nr, (color+add_color)/(sample_nr+1));
        sample_nr++;
        color += add_color;
    }
    return color/sample_nr;
}

void main()
{
    //vec3 reversedTexCoord = vec3(1.0 - TexCoord.x, 1.0 - TexCoord.y, TexCoord.z);
    //vec3 direction = normalize(vec3(0.5f, 0.5f, 0.5f) - reversedTexCoord);
    switch(methode){
    case 0:
        FragColor = rayMarch();
        break;
    case 1:
        FragColor = monteCarlo();
        break;
    case 2:
        uint randomValue = pcg32_random_r(pcg32_global);
        FragColor = vec4(float(randomValue) / float(0xFFFFFFFFu), 0.0, 0.0, 1.0);
        //FragColor = vec4(pcg32_random_float(), pcg32_random_float(),pcg32_random_float(), pcg32_random_float());
        break;
    default:
        FragColor = vec4(modelPos, 1.f);
    }
}
