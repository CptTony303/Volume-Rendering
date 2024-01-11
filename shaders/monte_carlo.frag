#version 330 core

out vec4 FragColor;

in vec3 modelPos; //intersection with cube in model position

uniform mat4 model; //model matrix
uniform mat4 view; //view matrix

uniform sampler3D volume; //texture of the volume

uniform int samplesPerFrame;
uniform float randomizer;

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
    seed += 5.36256;
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

vec4 deltaTracking(vec3 w){
    vec3 x = modelPos;
    float mu = 10.f;
    int counter = 0;
    while(true){
        if(counter >= 500){
//            return vec4(0.2f, 0.3f, 0.3f, 1.0f);
            return vec4(1.f, 0.f, 0.f, 1.0f);
        }
        counter++;
        float rng1 = get_random_float();
        float rng2 = get_random_float();

        float t = - log (1 - rng1)/mu;
        x += t * w;

        vec3 texCoord = x+vec3(0.5f);
        float density = texture(volume, texCoord).r*mu;

        if(rng2 < density/mu){
            return vec4(transferFunction(density/mu),1.f);
        }
        if(distance(x,modelPos) > 1.8){ //out of volume (1.8 ist längste diagonale eines Würfels)
            return vec4(0.2f, 0.3f, 0.3f, 1.0f);
        }
    }
}


void main(){
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

    FragColor = color/sample_nr;
}